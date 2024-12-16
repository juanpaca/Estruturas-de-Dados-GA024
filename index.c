#include <stdio.h>
#include <stdlib.h>
/*Para ler carateres sem acentuacao*/
#include <string.h>
#include <ctype.h> /*Para função isalpha*/
#include "index.h" 

#define SIZEW 17 /*Tamanho da palavra*/

typedef struct word{
	char key[SIZEW]; /*palavras de 16 carateres*/
	int line; /*Descreve a posição onde está a palavra
	Por convenção cada palavra chave tem
	line = 0*/
	struct word* next; /*Colisao com listas*/
}Word;

struct index{
	Word** table;
	int size; /*Tamanho da tabela*/
	int countkey;	/*Key words na tabela*/
	char* key_file; /*Nome do argv[1]*/
	char* text_file; /*Nome do argv[2]*/
};

static Index* begin(Index* tab , int size){
/*Inicia a tabela com NULL*/
	tab = (Index*)malloc(sizeof(Index));
	tab->table = malloc(sizeof(Word*)*size);
	tab->size = size;
	tab->countkey = 0; 
	
	for(int i = 0; i<size; i++)
		tab->table[i] = NULL;

	return tab;
}

void index_destroy(Index** table){
	/*Deixo sem static para adicionar no index.h*/
	for (int i = 0; i < (*table)->size; i++) {
        Word* a = (*table)->table[i];
        while (a != NULL) {
            Word* temp = a;
            a = a->next;
            free(temp);
        }
    }
    /*Apaga a tabela*/
    free((*table)->table);
    free((*table)->text_file);
    free((*table)->key_file);
    free(*table);
    *table = NULL;
}

static int hash(const char* s, int size) {
    unsigned long hash = 0;
    int c;
    while ((c = *s++))
        hash = (hash*31  + c) % size;
    return hash;
}

static void rehash(Index** tab){
	/*So se for necessario*/
	int new = (*tab)->size * 2; /*Duplica tamanho da tabela*/
	Index* new_tab = begin(new_tab , new);

	for(int i = 0; i < (*tab)->size; i++){
		Word* a = (*tab)->table[i];
		while(a != NULL){
			Word* next = a->next;
			int h = hash(a->key , new); /*Novo índice*/
			a->next = (new_tab)->table[h];
			(new_tab)->table[h] = a;
			a = next;
		}
	}
	new_tab->countkey = (*tab)->countkey;
	free((*tab)->table);
	*tab = new_tab;
}

static Word* hash_access(Index* tab, char* s, int line){
	/*Função accesar palavras de text_file, se um
	elemento existe na mesma linha nao insere na tabela
	e devolve o ponteiro a palavra achada.
	Se nao insere*/
	int h = hash(s, tab->size);
	Word* a = tab->table[h];

	/*Test para evitar repetir na mesma linha*/

	while(a != NULL){
		if(a->line == line && strcmp(s,a->key)==0)
			return a;
		a = a->next;
	}
	/*Se s nao esta na tabela, entao adiciona*/
	a = (Word*)malloc(sizeof(Word));
	if(a==NULL){
		printf("mem. error");
		exit(-3);
	}
	
    strcpy(a->key, s); 
    a->line = line;
    a->next = tab->table[h];
	tab->table[h] = a;
	//(tab->count) +=1; /*Pode server para rehash*/

	return a;
}

static Word* hash_accesskey(Index* tab, char* s){
	/*Função accesar para palavras chave*/
	int h = hash(s, tab->size);
	Word* a = tab->table[h];

	/*Teste para evitar repetir palavra chave*/
	while(a != NULL){
		if(strcmp(a->key,s) == 0)
			return a; /*Vai retornar a última ocurrenca*/
		a = a->next;
	}
	/*Se s nao esta na tabela, entao adiciona*/
	a = (Word*)malloc(sizeof(Word));
	if(a==NULL){
		printf("mem. error");
		exit(-3);
	}
	
    strcpy(a->key, s); 
    a->line = 0;
    a->next = tab->table[h];
	tab->table[h] = a;
	(tab->countkey) +=1; /*Conta +1 chave*/

	return a;
}

static int read_word(FILE* file, char* s, int* line){
	/*Essa função lé exatamente uma palavra
	no sentido de que lé carateres (a-z e A-Z).
	Alem disso informa em qué linha está a palavra.*/

	int i = 0;
	int c;

	/*Encontra o primer carater alfabetico*/
	while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            (*line)++;
        } else if (isalpha(c)) {
            break;
        }
    }

	if(c == EOF)
		return 0;
	else
		s[i++] = tolower(c); /*minuscula*/

	/*Lé os carateres até achar outro nao alfabetico*/
	while (i < SIZEW -1 && (c = fgetc(file)) != EOF) {
        if (c == '\n') {
            (*line)++;
            break; // Encontra um novo salto de linha, termina a palavra atual
        	} else if (!isalpha(c)) {
            break; // Encontra um caractere não alfabético, termina a palavra atual
        }
        s[i++] = tolower(c); /*minuscula*/
    }
	s[i] ='\0';

	return 1;
}

static int read_wordkey(FILE* file, char* s){
	/*Essa função lé as palavras chave*/

	int i = 0;
	int c;

	/*Encontra o primer carater alfabetico*/
	while ((c = fgetc(file)) != EOF) {
         if (isalpha(c)) 
            break;
    }

	if(c == EOF)
		return 0;
	else{
		s[i++] = tolower(c); /*minuscula*/
	}

	/*Lé os carateres até achar outro nao alfabetico*/
	while( i < SIZEW - 1 && (c = fgetc(file)) != EOF && isalpha(c) ){
		 
		s[i++] = tolower(c); /*minuscula*/
	}
	s[i] ='\0';

	return 1;
}

int index_createfrom(const char *key_file, 
const char *text_file, Index **idx ){
	
	FILE* key = fopen(key_file,"rt");/*Abrir arquivo
	key_file para leitura em modo texto*/
	if(key == NULL){
		printf("Erro na abertura do arquivo");
		return -1;
	}
  	
  	FILE* text = fopen(text_file,"rt");/*Abrir arquivo
	key_file para leitura em modo texto*/
	if(key == NULL){
		printf("Erro na abertura do arquivo");
		return -2;
	}

	*idx = begin(*idx , 193);/*Tamanho inicial*/

	size_t lkey = strlen(key_file);
	size_t ltext = strlen(text_file);

	(*idx)->key_file = (char *)malloc((lkey + 1) * sizeof(char));
    if ((*idx)->key_file == NULL) {
        fprintf(stderr, "Mem. erro\n");
        return -3;
    }

    (*idx)->text_file = (char *)malloc((ltext + 1) * sizeof(char));
    if ((*idx)->text_file == NULL) {
        fprintf(stderr, "Mem. erro\n");
        return -3;
    }

	strcpy((*idx)->text_file , (char*)text_file);
	strcpy((*idx)->key_file , (char*)key_file);
    char s[SIZEW]; 

    while(read_wordkey(key , s)){
		hash_accesskey(*idx , s); 
	}
	
	int line = 1;
	while(read_word(text,s,&line)){
		int h = hash(s, (*idx)->size);
		Word* b = (*idx)->table[h];
		while(b != NULL){
			if (b != NULL){
				if(strcmp(s,b->key)==0){
					hash_access(*idx,s,line);
				}
			}
			b = b->next;
		}
	}
	
	fclose(key);
    fclose(text);
	return 0;
}

int index_get( const Index *idx, const char *key, 
	int **occurrences, int *num_occurrences ){
/*Ainda pode se melhorar com so um while fazendo realloc*/

	if( !idx || !key || !occurrences || !num_occurrences)
		return -1;

	/*Minuscula*/
	char key1[SIZEW];
	size_t i;
    for (i = 0; i < SIZEW - 1 && key[i] != '\0'; i++) {
        key1[i] = tolower((unsigned char)key[i]);
    }
    key1[i] = '\0';

	int h = hash(key1, idx->size); /*indice*/
	Word* a = idx->table[h];

	if(a == NULL)/*Nao existe table[h]*/
		return -10;

	*num_occurrences = 0;
	/*Numero de ocurrencias*/
	while(a != NULL){
		if (strcmp(key1, a->key) == 0){
			if(a->line != 0){
				(*num_occurrences)++;
			}
		}
		a = a->next;
	}

	if ((*num_occurrences) == 0){/*Nao achou palavras no texto*/
		*occurrences = NULL;
		return 0;
	}	
	
	*occurrences = (int*)calloc(*num_occurrences ,sizeof(int));
	if(*occurrences == NULL){
		printf("Mem. erro");
		return 1;
	}

	int index = *num_occurrences - 1;

	a = idx->table[h];
	
	while(a != NULL && index >= 0 ){
		if (strcmp(key1, a->key) == 0){
			if(a->line != 0){
				(*occurrences)[index] = a->line;
				index = index - 1;
			}
		}
		a = a->next; 
	}

	/*->Tem que liberar o vetor de ocurrences, onde?<-*/
	return 0;
}

static Word* word_remove(Word* l, char* key) {
    Word* ant = NULL;
    Word* p = l;

    /* Procurando o elemento */
    while (p != NULL && strcmp(p->key , key) != 0) {
        ant = p;
        p = p->next;
    }

    /* Achou?*/
    if (p == NULL)
        return l; /* Nao */

    if (ant == NULL) {/*Inicio*/
        l = p->next;
    } else {
        ant->next = p->next;
    }

    free(p);
    return l;
}

int index_put(Index *idx, const char *key){

	/*Procurar key e fazer minuscula*/
	char key1[SIZEW];
	size_t i;
    for (i = 0; i < SIZEW - 1 && key[i] != '\0'; i++) {
        key1[i] = tolower((unsigned char)key[i]);
    }
    key1[i] = '\0';

	Word* a = hash_accesskey(idx,key1);
	/*a retorna a ultima ocurrenca se existir
	se nao tem-se que a->line = 0*/
  	
	if(a->line != 0){ /*Se tem ocurrencas de key
		/*Deletar cada elemento que tem a->key*/
		int h = hash(key1 , idx->size);
		Word* b = idx->table[h];

		int num_occurrences = 0;
	/*Numero de ocurrencias*/
	while(b != NULL){
		if (strcmp(key1, b->key) == 0){
			if(b->line != 0){
				num_occurrences++;
			}
		}
		b = b->next;
	}

	for(int i = 0 ; i < num_occurrences ; i++)
		idx->table[h] = word_remove(idx->table[h] , key1);

	}

	FILE* text = fopen(idx->text_file,"rt");
	if(key == NULL){
		printf("\nErro na abertura do arquivo2");
		return -2;
	}

	/*Inserir novamente as ocurrencas na tabela*/
	int line = 1;
	char s[SIZEW];
	while(read_word(text,s, &line)){
		if(strcmp(s,key1)==0){
			hash_access(idx , s ,line);
		}
	}

	fclose(text);

	return 0;
}

static void print_word(const Index *idx, const char *key){
	/*Imprime: word_example: n1, n2,...*/
	int *occurr;
	int num;
	if(!index_get( idx, key, 
				&occurr, &num)){
	if(num == 0)
		return;
	
	printf("%s: ", key);

	for(int j = 0; j < num-1 ; j++){
		printf("%d, ",occurr[j]);
		}
	}
	printf("%d\n",occurr[num-1]);

	free(occurr);
}

static int compar(const void *key1 , const void *key2){
	/*Orden alfabetico*/
	const char *p1 = *(const char **)key1;
	const char *p2 = *(const char **)key2;
	return strcmp(p1, p2);
}

int index_print(const Index *idx){

	char** keys = (char**)calloc(idx->countkey ,sizeof(char*));
	if(keys == NULL){
		printf("Mem. erro");
		exit(-2);
	}
	*keys[SIZEW];
	int k = 0;

	for (int i = 0; i < idx->size && k < idx->countkey ; ++i ){
		Word* a = idx->table[i];

		while(a != NULL){
			if(a->line == 0){
				keys[k] = a->key;
				k++;
			}
			a = a->next;
		}
	}

	/*orden alfabetico do keys*/
	qsort(keys , idx->countkey , sizeof(*keys) , compar);

	for(int j = 0; j < idx->countkey ; j++)
		print_word(idx , keys[j]);

	free(keys);  
	return 0;
}

static void print_visu(Index* idx){
	/*Funcao para visualizar a tabela, so para testes*/
    for (int i = 0; i < idx->size; i++) {
    Word* a = idx->table[i];
    while (a != NULL) {
        printf("(%s , %d)", a->key, a->line);
        a = a->next;
        if (a != NULL) {
            printf("->");
        }
    }
    printf("->NULL\n");
    }
}


