/* Created on 2016-08-30
 * Author: Zhang Binbin
 */
#include <stdio.h>

#include "matrix.h"

void TestMatrixAdd() {
    Matrix mat1, mat2;
    int r, c;
    float tmp;
    scanf("%d%d", &r, &c);
    mat1.Resize(r, c);
    mat2.Resize(r, c);
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            scanf("%f", &tmp); 
            mat1(i, j) = tmp;
        }
    }
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            scanf("%f", &tmp); 
            mat2(i, j) = tmp;
        }
    }

    mat1.Add(mat2);
    mat1.Print();
}

void TestMatrixMul() {
    Matrix mat1, mat2;
    int r1, c1;
    scanf("%d%d", &r1, &c1);
    mat1.Resize(r1, c1);
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c1; j++) {
            scanf("%f", &mat1(i, j)); 
        }
    }
    
    int r2, c2;
    scanf("%d%d", &r2, &c2);
    mat2.Resize(r2, c2);
    for (int i = 0; i < r2; i++) {
        for (int j = 0; j < c2; j++) {
            scanf("%f", &mat2(i, j));
        }
    }
    Matrix mat3;
    mat3.Mul(mat1, mat2);
    mat3.Print();
}

void TestMatrixInverse() {
    Matrix mat1, mat2;
    int r1, c1;
    scanf("%d%d", &r1, &c1);
    mat1.Resize(r1, c1);
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c1; j++) {
            scanf("%f", &mat1(i, j)); 
        }
    }

    mat2.Inverse(mat1);
    mat2.Print();
}

void TestComplexMatrixAdd() {
    ComplexMatrix mat1, mat2; 
    int r1, c1;
    scanf("%d%d", &r1, &c1);
    mat1.Resize(r1, c1);
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c1; j++) {
            scanf("%f + %fi", &mat1(i, j).real, &mat1(i, j).img); 
        }
    }
    //mat1.Print();

    mat2.Resize(r1, c1);
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c1; j++) {
            scanf("%f + %fi", &mat2(i, j).real, &mat2(i, j).img); 
        }
    }
    //mat2.Print();
    
    mat1.Add(mat2);
    mat1.Print();
}

void TestComplexMatrixMul() {
    ComplexMatrix mat1, mat2; 
    int r1, c1;
    scanf("%d%d", &r1, &c1);
    mat1.Resize(r1, c1);
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c1; j++) {
            scanf("%f + %fi", &mat1(i, j).real, &mat1(i, j).img); 
        }
    }
    
    int r2, c2;
    scanf("%d%d", &r2, &c2);
    mat2.Resize(r2, c2);
    for (int i = 0; i < r2; i++) {
        for (int j = 0; j < c2; j++) {
            scanf("%f + %fi", &mat2(i, j).real, &mat2(i, j).img); 
        }
    }
   
    ComplexMatrix mat3;
    mat3.Mul(mat1, mat2);
    mat3.Print();
}

void TestComplexMatrixInverse() {
    ComplexMatrix mat1, mat2; 
    int r1, c1;
    scanf("%d%d", &r1, &c1);
    mat1.Resize(r1, c1);
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c1; j++) {
            scanf("%f + %fi", &mat1(i, j).real, &mat1(i, j).img); 
        }
    }
    //mat1.Print();

    mat2.Inverse(mat1);
    mat2.Print();
}

int main() {
    //TestMatrixAdd(); 
    //TestMatrixMul();
    TestMatrixInverse();
    //TestComplexMatrixAdd();
    //TestComplexMatrixMul();
    //TestComplexMatrixInverse();
    return 0;
}



