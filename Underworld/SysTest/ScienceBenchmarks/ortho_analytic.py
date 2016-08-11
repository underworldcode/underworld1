#-----------------------------
# Analytic Result:
#

E = [-2, 2, 0]

# 45 degrees
n1 = -0.707106781186548;
n2 = -0.707106781186548;

# Hmmm - should look at using Numpy rather than defining local versions of
#  these.
def vec_mat_mult(vec, mat):
    res = [0 for ii in vec]    
    for resI in range(len(vec)):
        # Along the vec row, down the matrix col
        for colI in range(len(mat)):
            vecI = colI
            rowI = resI
            res[resI] += vec[vecI] * mat[colI][rowI]
    return res

def calc_A_matrix(C_h11, C_h22, C_h33, C_h12, C_h13, C_h23):
    """Calculates A = [C_00 C_01 C_02; C_01 C_11 C_12; C_02 C_12 C_22]"""
    C_00 = n1*n1*n1*n1*C_h11+2*n1*n1*n2*n2*C_h12-2*n1*n1*n1*n2*C_h12+n2*n2*n2*n2*C_h22-4*n2*n2*n2*n1*C_h23-2*n1*n1*n1*n2*C_h13+4*n1*n1*n2*n2*C_h33
    C_01 = n1*n1*n2*n2*C_h11+n2*n2*n2*n2*C_h12-2*n1*n2*n2*n2*C_h12+n1*n1*n1*n1*C_h12+n2*n2*n1*n1*C_h22-2*n1*n1*n1*n2*C_h23+2*n1*n1*n1*n2*C_h13+2*n2*n2*n2*n1*C_h23-4*n1*n1*n2*n2*C_h33
    C_02 = n1*n1*n1*n2*C_h11+n1*n2*n2*n2*C_h12-2*n1*n1*n2*n2*C_h12-n1*n1*n1*n2*C_h12-n1*n2*n2*n2*C_h22+3*n1*n1*n2*n2*C_h23+n1*n1*n1*n1*C_h13-n1*n1*C_h13*n2*n2-n2*n2*n2*n2*C_h23-2*n1*n1*n1*n2*C_h33+2*n1*n2*n2*n2*C_h33
    C_11 = n2*n2*n2*n2*C_h11+2*n1*n1*n2*n2*C_h12+2*n1*n2*n2*n2*C_h12+n1*n1*n1*n1*C_h22+4*n1*n1*n1*n2*C_h23+2*n1*n2*n2*n2*C_h13+4*n1*n1*n2*n2*C_h33
    C_12 = n1*n2*n2*n2*C_h11+n1*n1*n1*n2*C_h12+2*n1*n1*n2*n2*C_h12-n1*n2*n2*n2*C_h12-n1*n1*n1*n2*C_h22-3*n1*n1*n2*n2*C_h23+n1*n1*C_h13*n2*n2-n2*n2*n2*n2*C_h13+n1*n1*n1*n1*C_h23+2*n1*n1*n1*n2*C_h33-2*n1*n2*n2*n2*C_h33
    C_22 = n1*n1*n2*n2*C_h11-2*n1*n1*n2*n2*C_h12+n1*n1*n1*n2*C_h12-n1*n2*n2*n2*C_h12+n2*n2*n1*n1*C_h22-2*n1*n1*n1*n2*C_h23+2*n2*n2*n2*n1*C_h23+n1*n1*n1*n2*C_h13-n1*n2*n2*n2*C_h13+C_h33*n1*n1*n1*n1-2*n1*n1*n2*n2*C_h33+C_h33*n2*n2*n2*n2
    A = [[C_00, C_01, C_02], [C_01, C_11, C_12], [C_02, C_12, C_22]]
    return A

def calc_stress(C_h11, C_h22, C_h33, C_h12, C_h13, C_h23):
    A = calc_A_matrix(C_h11, C_h22, C_h33, C_h12, C_h13, C_h23)
    stress = vec_mat_mult(E, A)
    return stress

if __name__ == "__main__":
    # Case 1:
    # A = [550 450 0; 450 550 0; 0 0 500]
    # Case 2:
    # A = [510 490 0; 490 510 0; 0 0 500]
    # Case 2:
    # A = [501 499 0; 499 501 0; 0 0 500] 
    # stress = E*A
    # Case 1:
    # stress = [-200 200 0]
    # Case 2:
    # stress = [-40 40 0]
    # Case 3:
    # stress = [-4 4 0]

    # Case 1:
    C_h11 = 1000
    C_h22 = 1000
    C_h33 = 50
    C_h12 = 0
    C_h13 = 0
    C_h23 = 0
    A1 = calc_A_matrix(C_h11, C_h22, C_h33, C_h12, C_h13, C_h23)
    stress1 = vec_mat_mult(E, A1)

    # Case 2:
    C_h11 = 1000
    C_h22 = 1000
    C_h33 = 10
    C_h12 = 0
    C_h13 = 0
    C_h23 = 0
    A2 = calc_A_matrix(C_h11, C_h22, C_h33, C_h12, C_h13, C_h23)
    stress2 = vec_mat_mult(E, A2)

    # Case 3:
    C_h11 = 1000
    C_h22 = 1000
    C_h33 = 1
    C_h12 = 0
    C_h13 = 0
    C_h23 = 0
    A3 = calc_A_matrix(C_h11, C_h22, C_h33, C_h12, C_h13, C_h23)
    stress3 = vec_mat_mult(E, A3)
