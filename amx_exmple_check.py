import numpy as np

ROW_A, COL_A = 16, 64
ROW_B, COL_B = 64, 16
ROW_C, COL_C = 16, 16

A = np.fromfunction(lambda i, j: (i + j) % 255, (ROW_A, COL_A), dtype=np.int32)
B = np.fromfunction(lambda i, j: (i * j) % 255, (ROW_B, COL_B), dtype=np.int32)
C = np.fromfunction(lambda i, j: i * j, (ROW_C, COL_C), dtype=np.int32)

C += A @ B

print(C)
