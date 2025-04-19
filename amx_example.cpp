#include <cstring>
#include <immintrin.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "common.h"

template<typename T, std::size_t N>
void print_matrix(const std::array<T, N> &matrix, std::size_t rows, std::size_t cols, const std::string &label = "") {
    if (rows * cols != N) {
        std::cerr << "Error: Size mismatch (rows * cols != matrix.size()).\n";
        return;
    }

    if (!label.empty()) {
        std::cout << label << ":\n";
    }

    for (std::size_t i = 0; i < rows; ++i) {
        for (std::size_t j = 0; j < cols; ++j) {
            std::cout << std::setw(8) << static_cast<int64_t>(matrix[i * cols + j]);
        }
        std::cout << '\n';
    }
    std::cout << std::endl;
}

constexpr int XFEATURE_XTILECFG = 17;
constexpr int XFEATURE_XTILEDATA = 18;
constexpr int XFEATURE_MASK_XTILECFG = 1 << XFEATURE_XTILECFG;
constexpr int XFEATURE_MASK_XTILEDATA = 1 << XFEATURE_XTILEDATA;
constexpr int XFEATURE_MASK_XTILE = XFEATURE_MASK_XTILECFG | XFEATURE_MASK_XTILEDATA;
constexpr int ARCH_GET_XCOMP_PERM = 0x1022;
constexpr int ARCH_REQ_XCOMP_PERM = 0x1023;

bool init_amx() {
    unsigned long bitmask = 0;
    long status = syscall(SYS_arch_prctl, ARCH_GET_XCOMP_PERM, &bitmask);
    if (0 != status) return false;
    if (bitmask & XFEATURE_MASK_XTILEDATA) return true;

    status = syscall(SYS_arch_prctl, ARCH_REQ_XCOMP_PERM, XFEATURE_XTILEDATA);
    if (0 != status)
        return false; // XFEATURE_XTILEDATA setup is failed, TMUL usage is not allowed
    status = syscall(SYS_arch_prctl, ARCH_GET_XCOMP_PERM, &bitmask);

    // XFEATURE_XTILEDATA setup is failed, can't use TMUL
    if (0 != status || !(bitmask & XFEATURE_MASK_XTILEDATA)) return false;

    // XFEATURE_XTILEDATA set successfully, TMUL usage is allowed
    return true;
}

//Define tile config data structure

struct amx_tile_config {
    uint8_t palette_id;
    uint8_t start_row;
    uint8_t reserved_0[14];
    uint16_t colsb[8];
    uint16_t reserved_1[8];
    uint8_t rows[8];
    uint8_t reserved_2[8];
};

static_assert(sizeof(amx_tile_config) == 64);
constexpr int AMX_ROW = 16;
constexpr int AMX_COLB = 64;
constexpr int AMX_COL_INT8 = AMX_COLB / sizeof(uint8_t);
constexpr int AMX_COL_INT32 = AMX_COLB / sizeof(int32_t);
constexpr int ROW_A = AMX_ROW;
constexpr int COL_A = AMX_COL_INT8;
constexpr int ROW_B = AMX_COL_INT8;
constexpr int COL_B = AMX_ROW;
constexpr int ROW_C = AMX_ROW;
constexpr int COL_C = AMX_COL_INT32;


void config_amx_tile() {
    amx_tile_config cfg = {};

    cfg.palette_id = 1;
    cfg.start_row = 0;

    cfg.rows[0] = AMX_ROW;
    cfg.rows[1] = AMX_ROW;
    cfg.rows[2] = AMX_ROW;

    cfg.colsb[0] = AMX_COLB;
    cfg.colsb[1] = AMX_COLB;
    cfg.colsb[2] = AMX_COLB;

    _tile_loadconfig(&cfg);
}

void reorder_B(uint8_t *B) {
    constexpr int k_pack = 4 / sizeof(uint8_t);

    // tmp in [k/k_pack][COL_B][k_pack]
    std::array<uint8_t, ROW_B * COL_B> tmp = {};

    for (int k = 0; k < ROW_B; ++k) {
        for (int n = 0; n < COL_B; ++n) {
            //  tmp[k/k_pack][n][k%k_pack] = B[k][n]
            int k_group = k / k_pack;
            int k_offset = k % k_pack;
            tmp[(k_group * COL_B + n) * k_pack + k_offset] = B[k * COL_B + n];
        }
    }

    std::memcpy(B, tmp.data(), ROW_B * COL_B * sizeof(uint8_t));
}

void amx_mat_mul(int32_t *C, uint8_t *A, uint8_t *B) {
    reorder_B(B);
    _tile_loadd(0, A, AMX_COLB);
    _tile_loadd(1, B, AMX_COLB);
    _tile_loadd(2, C, AMX_COLB);
    _tile_dpbuud(2, 0, 1); // C += A * B
    _tile_stored(2, C, AMX_COLB);
}

int main() {
    std::array<uint8_t, ROW_A * COL_A> A = {};
    std::array<uint8_t, ROW_B * COL_B> B = {};
    std::array<int32_t, ROW_C * COL_C> C = {};

    // init A (16x64) in row major order
    for (int i = 0; i < ROW_A; ++i) {
        for (int j = 0; j < COL_A; ++j) {
            A[i * COL_A + j] = (i + j) % 255;
        }
    }

    // init B (64x16) in row major order
    for (int i = 0; i < ROW_B; ++i) {
        for (int j = 0; j < COL_B; ++j) {
            B[i * COL_B + j] = (i * j) % 255;
        }
    }

    // init C (16x16) in row major order
    for (int i = 0; i < ROW_C; ++i) {
        for (int j = 0; j < COL_C; ++j) {
            C[i * COL_C + j] = i * j;
        }
    }

    init_amx();
    config_amx_tile();
    amx_mat_mul(C.data(), A.data(), B.data());
    print_matrix(C, AMX_ROW, AMX_COL_INT32, "C");
}
