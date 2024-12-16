
#include<stdio.h>
#include<stdlib.h>
#include "matrix.h"


static int add_element_column(Matrix** mat){
    
        Matrix* novor = (Matrix*)malloc(sizeof(Matrix));
        if (novor == NULL) {
            printf("Mem. erro\n");
            return 1;
        }

        novor->line = 0;
        novor->column = -1;

        if ((*mat)->right->line == -1){
            (*mat)->right = novor;
            novor->right = *mat;
            novor->below = novor;
            return 0;
        }

        Matrix *p = (*mat)->right;

        novor->right = p;
        (*mat)->right = novor;
        novor->below = novor;
        return 0;  
}

static int add_element_row(Matrix** mat){
    
        Matrix* novor = (Matrix*)malloc(sizeof(Matrix));
        if (novor == NULL) {
            printf("Mem. erro\n");
            return 1;
        }

        novor->line = -1;
        novor->column = 0; 

        if ((*mat)->below->column == -1){
            (*mat)->below = novor;
            novor->below = *mat;
            novor->right = novor;
            return 0;
        }

        Matrix *p = (*mat)->below;

        novor->below = p;
        (*mat)->below = novor;
        novor->right = novor;
        return 0; 
}

static int matrix_row_number(const Matrix* m){
    int row = 0;

    Matrix* q = m->below;
    while(q != m){
        row++;
        q = q->below;
    }

    return row;
}

static int matrix_column_number(const Matrix* m){
    int colm = 0;

    Matrix* q = m->right;
    while(q != m){
        colm++;
        q = q->right;
    }

    return colm;
}

int matrix_setelem(Matrix* m, int x, int y, float elem) {
    /*Casos base*/
    if (x > matrix_row_number(m) || x<=0 || y > matrix_column_number(m)|| y<=0 )  
       return 0;    

    Matrix *pCol = m->right;
    Matrix *pLin = m->below;

    /*Percorrendo até linha x e coluna y*/
    for(int i=1; i<= x-1; i++)
        pLin = pLin->below;

    for(int i=1; i<= y-1; i++)
        pCol = pCol->right; 
    
    Matrix *actuall = pLin->right;
    Matrix *antl = pLin;

    /*(x,y) existe na matriz o nao?*/
    while(actuall->column < y && actuall->line != -1){
        antl = actuall;
        actuall = actuall->right;
    }

    /*(x,y) existe na matriz..*/
    if (actuall->column == y){

        /*Si (x,y,!=0) mudamos o info*/
        if (elem != 0){
            actuall->info = elem;
            return 0;
        }

        /*Si (x,y,0) reconetar e apagar o existente*/
        
        Matrix *actualp = pCol->below;
        Matrix *antp = pCol;

        while(actualp->line < x){
            antp = actualp;
            actualp = actualp->below;
        }
        
        antl->right = actuall->right;
        antp->below = actualp->below;        
        free(actuall);
        return 0;
    }

    /*Se (x,y) nao existe na matriz..*/

        /*Se vamos inserir (x,y,0) nao faz nada..*/
    if (elem == 0)
        return 0;

        /*Se vamos inserir (x,y,!=0)..*/
    Matrix* novo = (Matrix*)malloc(sizeof(Matrix));
    if (novo == NULL) {
        printf("Mem. erro\n");
        return 1;
    }

    novo->line = x;
    novo->column = y;
    novo->info = elem;

    Matrix *actualp = pCol->below;
    Matrix *antp = pCol;

    while(actualp->line < x && actualp->column!= -1){
        antp = actualp;
        actualp = actualp->below;
    }

    antl->right = novo;
    novo->right = actuall;
    antp->below = novo;
    novo->below = actualp;

    return 0;
}

static int matrix_empty_create(Matrix** mat, int linhas, int colunas){
    
    if (mat == NULL) return 1;

    *mat = (Matrix*)malloc(sizeof(Matrix));
    if (*mat == NULL){ 
        printf("Mem. erro");
        return 1;
    }

        (*mat)->right = *mat;
        (*mat)->below = *mat;
        (*mat)->line = -1;
        (*mat)->column = -1;
        (*mat)->info = 0;

    for (int i = 1; i <= colunas; i++){
        add_element_column(mat);
    }
    
    for (int i = 1; i <= linhas; i++){
        add_element_row(mat);
    }

    return 0;
}

int matrix_create(Matrix** mat){
   
    /*0.1. Alocação do nó (-1,-1)*/
    *mat = (Matrix*)malloc(sizeof(Matrix));
    if (*mat == NULL){ 
        printf("Mem. erro");
        return 1;
    }

        /*0.3. Data inicial de mat*/
        (*mat)->right = *mat;
        (*mat)->below = *mat;
        (*mat)->line = -1;
        (*mat)->column = -1;
        (*mat)->info = 0;


        /*1. Letura do tamanho da matriz*/
    int linhas, colunas;
    scanf("%d %d", &linhas, &colunas);
    if (linhas <= 0 || colunas <= 0) {
        Matrix *p = *mat;
        (*mat)->right = NULL;
        (*mat)->below = NULL;
        free(p);
        *mat = NULL;        
        return 1;
    }

    /*1.1. Criação de lista right e lista below*/
        /*Lista circular das colunas*/
    for (int i = 1; i <= colunas; i++){
        add_element_column(mat);
    }
   
    for (int i = 1; i <= linhas; i++){
        add_element_row(mat);
    }

        /*2. Inserção de elementos da matriz*/
    
    for (int i = 1; i <= linhas*colunas; i++){
           int line;
        scanf("%d",&line);
        if (line <= 0 || line > linhas ) return 0;

        int column;
        scanf("%d",&column);
        if ( column <= 0 || column > colunas) return 0;

        float info;
        scanf("%f",&info);
        matrix_setelem(*mat, line, column, info);
        }          

    return 0;
}

int matrix_getelem(const Matrix* m, int x, int y, float *elem){
    Matrix* p = m->below;

    for(int i = 1 ; i <= x-1; i++)
        p = p->below;

    do{
        p = p->right;
        if (p->column == y){
            *elem = p->info;
            return 0;
        }
    }while(p->right->line != -1);

    *elem = 0;
    return 1;
}

int matrix_destroy( Matrix* m){
    /*É tarefa do usuario fazer NULL no main*/
    if(m == NULL)
        return 1;

    Matrix* q = m->below;
    Matrix* temp = NULL;
    Matrix* p = NULL;

    while(q->column != -1){
        p = q->right;
        Matrix* ant = NULL;

        while(p->line != -1){
        ant = p;
        p = p->right;
        free(ant);
        }

        temp = q;
        q = q->below;
        free(temp);
    }

    q = m->right;
    while(q->line != -1){
        temp = q;
        q = q->right;
        free(temp);
    }
    
     m->right = NULL;
     m->below = NULL;
     m->line = 0;
     m->column = 0;

     free(q);
     m = NULL;

    return 0;
}

int matrix_print(const Matrix* m){

    printf("\n%d %d\n", matrix_row_number(m), matrix_column_number(m));
    for (Matrix* p = m->below; p != m; p = p->below) {
        for (Matrix* q = p->right; q != p; q = q->right) {
            printf("%d %d %.5f", q->line, q->column, q->info);
            printf("\n");
        }
    }
    printf("0\n");
    return 0;
}  

int matrix_transpose( const Matrix* m, Matrix** r){

    /*Linhas e colunas de m*/
    int linhas = matrix_row_number(m);
    int colunas = matrix_column_number(m);


    if (!matrix_empty_create(r,colunas,linhas)){

        for (int i = 1; i <=colunas ; ++i)
        {
            for (int j = 1; j <= linhas; ++j)
            {
                float info;
                matrix_getelem(m, j, i, &info);

                matrix_setelem(*r, i, j,info);
            }
        }


    return 0;
    }

    return 1;
}

int matrix_add( const Matrix* m, const Matrix* n, Matrix** r){

    /*Linhas e colunas de m*/
    int linm = matrix_row_number(m);
    int colm = matrix_column_number(m);

    /*Linhas e colunas de n*/
    int linn = matrix_row_number(n);
    int coln = matrix_column_number(n);

    if ((linm != linn)||(coln != colm))
        return 1;

    if (!matrix_empty_create(r,linm,colm)){

        for (int i = 1; i <=linm ; ++i )
        {
            for (int j = 1; j <= colm; ++j)
            {
                float x;
                matrix_getelem(m, i, j, &x);

                float y;
                matrix_getelem(n, i, j, &y);

                matrix_setelem(*r, i, j,x+y);

            }
        }

        return 0;  
    }
    return 1;
}

int matrix_multiply( const Matrix* m, const Matrix* n, Matrix** r){

    /*linhas e colunas de m*/
    int linm = matrix_row_number(m);
    int colm = matrix_column_number(m); 

    /*linhas e colunas de n*/
    int linn = matrix_row_number(n);
    int coln = matrix_column_number(n);
 
    if (colm != linn) return 1;

    if (!matrix_empty_create(r,linm,coln)){
        float C = 0;

        for (int i = 1 ; i <= linm; i++){
            for (int j = 1; j <= coln; j++){
                for (int k = 1; k <= colm; k++){
                    float A;
                    float B;
                    matrix_getelem(m,i,k,&A);
                    matrix_getelem(n,k,j,&B);
                    C = C + A*B;
                }
                matrix_setelem(*r,i,j,C);
                C = 0;
            }
        }
        return 0;
    } 
}
