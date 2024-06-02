#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_QUESTIONS 10 // Jumlah pertanyaan Quiz

FILE *fp;
int m;
int totalScore = 0;
struct playerData temp;
struct Node* stack = NULL;

// Struct untuk pertanyaan quiz 
typedef struct {
    char question[256];
    char correct_answer[64];
} Question;

// Struct untuk data player
struct playerData{
    char player[100];
    int score, totalScore;
};

// Struct untuk node
struct Node {
    char data[100];
    struct Node* next;
};

int menu(){	
	printf("=== MENU ===\n");
	printf("1. Convert Expressions\n");
	printf("2. Evaluation\n");
	printf("3. Quiz\n");
	printf("0. Exit\n");
	printf("\n");
	
	do {
        printf("Input number: ");
        scanf("%d", &m);
    } while (m < 0 || m > 3);
    
    system("@cls||clear");
    
    return m;
}

// Menambahkan operator ke stack
void push(struct Node** top, char* data) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node)); // Create New Node
    strcpy(newNode->data, data); // Copy String data ke data milik new node
    newNode->next = *top; // Menambahkan	 next new node ke stack paling atas
    *top = newNode; // Stack paling atas adalah new node
}

// Mengeluarkan Operator dari stack
void pop(struct Node** top, char* result) {
    if (*top == NULL) return; // Jika stack paling atas adalah Null, maka function akan return tanpa melakukan apapun.
    struct Node* temp = *top; // Buat variable temp berada di atas sehingga temp bisa jalan-jalan
    strcpy(result, temp->data); // Copy data dari stack paling atas (temp) ke buffer result
    *top = (*top)->next; //merubah posisi top menjadi yang ada dibawahnya(top ke next)
    free(temp); // free temp
}

// Kalau mau tau stack nya kosong atau tidak 
int isEmpty(struct Node* top) {
    return top == NULL;
}

// Untuk mengatur "tingkatan/level" dari masing2 operators. semakin besar return, maka semakin tinggi "tingkat/level"nya.
int prec(char c) {
	return (c == '^') ? 3 : (c == '/' || c == '*') ? 2 : (c == '+' || c == '-') ? 1 : -1;
}

/* Untuk mengatur apabila ada pangkat (^) maka dua angka diantara pangkat akan dipangkatkan
 (dilihat dari yang paling kanan/ right-associative) . Example : 2 ^ 3 ^ 2 => 2 ^ (3 ^ 2) */
char associativity(char c) {
	return (c == '^') ? 'R' : 'L'; // Default left-associative
}

void infixToPostfix(char s[]) {
	//initialize variable
    char result[200];
    int resultIndex = 0;
    int len = strlen(s);
    int i;
    
    // Looping per-characters
    for (i = 0; i < len; i++) {
        char c = s[i]; // Ambil karakter saat ini.
        if (isalnum(c)) { // Kalau characters yang di input adalah operan, maka akan dimasukkan ke result array
            result[resultIndex++] = c;
        } else if (c == '(') { 
            char temp[2] = {c, '\0'}; // index pertama adalah character(String) yang di input, index kedua adalah untuk menandakan bahwa String tersebut valid.
            push(&stack, temp); // Kalau character yang di input adalah '(' maka akan di masukkan (push) ke stack
        } else if (c == ')') {
            char temp[100];
            while (!isEmpty(stack)) { // Kalau characters yang di input adalah ')' maka keluarkan (pop) elemen yang tersisa ke result array hingga bertemu dengan '(' lagi.
                pop(&stack, temp);
                if (temp[0] == '(') break;
                result[resultIndex++] = temp[0];
            }
        } else {
            char temp[100];
            while (!isEmpty(stack)) {
                pop(&stack, temp);
                if (prec(c) > prec(temp[0]) || (prec(c) == prec(temp[0]) && associativity(c) == 'R')) { // Kalau operators yang di input lebih kecil atau sama dengan operators yang sudah ada di stack sebelumnya, maka masukkan operators ke dalam stack
                    push(&stack, temp);
                    break;
                } else {
                    result[resultIndex++] = temp[0]; // Kalau operators yang di input lebih besar dari operators yang sudah ada di stack, maka keluarkan ke result Array.
                }
            }
            char temp2[2] = {c, '\0'};
            push(&stack, temp2);
        }
    }
    
    while (!isEmpty(stack)) { // Setelah memproses seluruh expression yang dimasukkan, keluarkan elemen yang tersisa di stack ke result array.
        char temp[100];
        pop(&stack, temp);
        result[resultIndex++] = temp[0];
    }

    result[resultIndex] = '\0'; // Null-terminator untuk membuat String menjadi valid
    printf("Result : %s\n", result); // Display risult
}

void postfixToPrefix(char s[]) {
    int len = strlen(s);
    int i;
    char result[100];

    for (i = 0; i < len; i++) { // Looping per-karakter
        char c = s[i]; // Ambil karakter saat ini.
        // Jika karakter adalah operand (huruf atau angka), dorong ke stack.
        if (isalnum(c)) {
            char operand[2] = {c, '\0'};
            push(&stack, operand);
        } else {
        	// Jika karakter adalah operator, keluarkan dua operand dari stack.
            char op1[100], op2[100];
            pop(&stack, op1);
            pop(&stack, op2);
            // Buat expression dengan operator yang diikuti oleh dua operand. contoh : +ab
            char expr[100] = {c, '\0'};
            strcat(expr, op2);
            strcat(expr, op1);
            push(&stack, expr); // Masukkan expression ke stack.
        }
    }
    pop(&stack, result); // Keluarkan elemen yang ada di stack
    printf("Result: %s\n", result); // Display result
}

void postfixToInfix(char s[]) {
    int len = strlen(s);
    int i;
    char result[100];
    
    for (i = 0; i < len; i++) { // Looping per-karakter
        char c = s[i]; // Ambil karakter saat ini.
        // Jika karakter adalah operand (huruf atau angka), dorong ke stack.
        if (isalnum(c)) {
            char operand[2] = {c, '\0'};
            push(&stack, operand);
        } else {
            char op1[100], op2[100]; // Jika karakter adalah operator, keluarkan dua operand dari stack.
            pop(&stack, op1);
            pop(&stack, op2);
            char expr[100] = "("; // Template buat infix expression (diawalin '(')
            strcat(expr, op2); // Masukkan operand pertama
            char opStr[2] = {c, '\0'};
            strcat(expr, opStr); // Masukkan operators
            strcat(expr, op1); // Masukkan operand selanjutnya
            strcat(expr, ")"); // Infix Expression ditutup oleh ')'
            push(&stack, expr); // Masukkan expression ke stack.
        }
    }
    pop(&stack, result); // Keluarkan elemen yang ada di stack
    printf("Result: %s\n", result); // Display result
}

void infixToPrefix(char s[]) {
    int len = strlen(s);
    int i;
    char rev[100];
    // Reverse String yang di input
    for (i = 0; i < len; i++) rev[i] = s[len - 1 - i];
    rev[len] = '\0';
    // Tukar setiap '(' dengan ')' begitupun sebaliknya
    for (i = 0; i < len; i++) {
        if (rev[i] == '(') {
            rev[i] = ')';
        } else if (rev[i] == ')') {
            rev[i] = '(';
        }
    }
    // Convert Reversed Infix to Postfix
    char postfix[100];
    int resultIndex = 0;
    
    for (i = 0; i < len; i++) { // Looping per-karakter
        char c = rev[i]; // Ambil karakter saat ini.
        if (isalnum(c)) { // Kalau yang di input adalah operand, maka taruh ke postfix array
            postfix[resultIndex++] = c;
        } else if (c == '(') { // Kalau operator yang di input adalah '(' maka taruh operatir ke stack
            char temp[2] = {c, '\0'};
            push(&stack, temp);
        } else if (c == ')') { 
            char temp[100];
            while (!isEmpty(stack)) { // Kalau characters yang di input adalah ')' maka keluarkan (pop) elemen yang tersisa ke result array hingga bertemu dengan '(' lagi.
                pop(&stack, temp);
                if (temp[0] == '(') break;
                postfix[resultIndex++] = temp[0];
            }
        } else {
            char temp[100];
            while (!isEmpty(stack)) {
                pop(&stack, temp);
                if (prec(c) > prec(temp[0]) || (prec(c) == prec(temp[0]) && associativity(c) == 'R')) { // Kalau operators yang di input lebih kecil atau sama dengan operators yang sudah ada di stack sebelumnya, maka masukkan operators ke dalam stack
                    push(&stack, temp);
                    break;
                } else {
                    postfix[resultIndex++] = temp[0]; // Kalau operators yang di input lebih besar dari operators yang sudah ada di stack, maka keluarkan ke postfix Array.
                }
            }
            char temp2[2] = {c, '\0'};
            push(&stack, temp2);
        }
    }
	// Setelah memproses seluruh expression yang dimasukkan, keluarkan elemen yang tersisa di stack ke postfix array.
    while (!isEmpty(stack)) { 
        char temp[100];
        pop(&stack, temp);
        postfix[resultIndex++] = temp[0];
    }
    postfix[resultIndex] = '\0';
    // Reverse Postfix Expression
    char prefix[1000];
    for (i = 0; i < resultIndex; i++) {
        prefix[i] = postfix[resultIndex - 1 - i];
    }
    prefix[resultIndex] = '\0';
    // Display Result
    printf("Result: %s\n", prefix); 
}

void prefixToPostfix(char s[]) {
    int len = strlen(s);
    int i;
    char result[100];
    char rev[100];
    // Reverse String yang di input
    for (i = 0; i < len; i++) {
        rev[i] = s[len - 1 - i];
    }
    rev[len] = '\0';
    for (i = 0; i < len; i++) { // Looping per-karakter
        char c = rev[i]; // Ambil karakter saat ini.
        // Jika karakter adalah operand (huruf atau angka), dorong ke stack.
        if (isalnum(c)) {
            char operand[2] = {c, '\0'};
            push(&stack, operand);
        } else {
        	// Jika karakter adalah operator, keluarkan dua operand dari stack.
            char op1[100], op2[100];
            pop(&stack, op1);
            pop(&stack, op2);
            // Buat expression dengan operator yang diikuti oleh dua operand. contoh : ba+
            char expr[100] = {'\0'};
            strcat(expr, op1);
            strcat(expr, op2);
            char opStr[2] = {c, '\0'};
            strcat(expr, opStr);
            push(&stack, expr); // Masukkan expression ke stack.
        }
    }
    pop(&stack, result); // Keluarkan elemen yang ada di stack
    printf("Result: %s\n", result); // Display result
}

void prefixToInfix(char s[]) {
    int len = strlen(s);
    int i;
    char rev[100], result[100];
    // Reverse String yang di input
    for (i = 0; i < len; i++) {
        rev[i] = s[len - 1 - i];
    }
    rev[len] = '\0';

    for (i = 0; i < len; i++) { // Looping per-karakter
        char c = rev[i]; // Ambil karakter saat ini.
        // Jika karakter adalah operand (huruf atau angka), dorong ke stack.
        if (isalnum(c)) {
            char operand[2] = {c, '\0'};
            push(&stack, operand);
        } else {
        	// Jika karakter adalah operator, keluarkan dua operand dari stack.
            char op1[100], op2[100];
            pop(&stack, op1);
            pop(&stack, op2);
            char expr[100] = "("; // Template buat infix expression (diawalin '(')
            strcat(expr, op1); // Masukkan operand pertama
            char opStr[2] = {c, '\0'};
            strcat(expr, opStr); // Masukkan operators
            strcat(expr, op2); // Masukkan operand selanjutnya
            strcat(expr, ")"); // Infix Expression ditutup oleh ')'
            push(&stack, expr); // Masukkan expression ke stack.
        }
    }
    pop(&stack, result);
    printf("Result: %s\n", result);
}

int converterMenu(){
	char exp[100];
	do{
		printf("====================\n");
		printf("== CONVERTER MENU ==\n");
		printf("====================\n");
		printf("1. Infix to Postfix\n");
		printf("2. Postfix to Prefix\n");
		printf("3. Postfix to Infix\n");
		printf("4. Infix to Prefix\n");
		printf("5. Prefix to Postfix\n");
		printf("6. Prefix to Infix\n");
		printf("0. Back to main menu\n");
		printf("\n");
	
		do{
			printf("Input number : ");
			scanf("%d", &m);		
		} while(m < 0 && m > 6);
		
		switch(m){
			case 1: 
				printf("================================\n");
				printf("== INFIX TO POSTFIX CONVERTER ==\n");
				printf("================================\n");
				printf("note : Expression cannot have a space\n");
				printf("input infix expression: ");
				scanf("%s", exp);
				infixToPostfix(exp);
				printf("\n");
				break;
			case 2:
				printf("=================================\n");
				printf("== POSTFIX TO PREFIX CONVERTER ==\n");
				printf("=================================\n");
				printf("note : Expression cannot have a space\n");
				printf("input postfix expression: ");
				scanf("%s", exp);
				postfixToPrefix(exp);
				printf("\n");
				break;
			case 3:
				printf("================================\n");
				printf("== POSTFIX TO INFIX CONVERTER ==\n");
				printf("================================\n");
				printf("note : Expression cannot have a space\n");
				printf("input postfix expression: ");
				scanf("%s", exp);
				postfixToInfix(exp);
				printf("\n");
				break;
			case 4:
				printf("===============================\n");
				printf("== INFIX TO PREFIX CONVERTER ==\n");
				printf("===============================\n");
				printf("note : Expression cannot have a space\n");
				printf("input infix expression: ");
				scanf("%s", exp);
				infixToPrefix(exp);
				printf("\n");
				break;
			case 5:
				printf("=================================\n");
				printf("== PREFIX TO POSTFIX CONVERTER ==\n");
				printf("=================================\n");
				printf("note : Expression cannot have a space\n");
				printf("input prefix expression: ");
				scanf("%s", exp);
				prefixToPostfix(exp);
				printf("\n");
				break;
			case 6:
				printf("===============================\n");
				printf("== PREFIX TO INFIX CONVERTER ==\n");
				printf("===============================\n");
				printf("note : Expression cannot have a space\n");
				printf("input prefix expression: ");
				scanf("%s", exp);
				prefixToInfix(exp);
				printf("\n");
				break;
			case 0:
				system("@cls||clear");
				return 0;
		}
	} while ( m != 0 );
	return;
}

void postfixEvaluate(char s[]) {	
    int stack[100]; // Stack untuk menampung elements
    int stackIndex = -1;
    char *token = strtok(s, " ");
    
    while(token != NULL){
    	int c = token[0];
    	if (isdigit(c)) {
            stack[++stackIndex] = c - '0'; // Jika input yang masuk adalah operand, maka masukkan ke stack
        } else { //Jika input yang masuk adalah operators
        	// Pop 2 operand
            int op1 = stack[stackIndex--]; 
            int op2 = stack[stackIndex--];
            int result;

            switch (c) {
                case '+':
                    result = op2 + op1;
                    break;
                case '-':
                    result = op2 - op1;
                    break;
                case '*':
                    result = op2 * op1;
                    break;
                case '/':
                    result = op2 / op1;
                    break;
                case '^':
                    result = pow(op2, op1);
                    break;
                default:
                    printf("Invalid operator encountered: %c\n", c);
                    return;
            }
            stack[++stackIndex] = result; // Masukkan result ke stack
        }
        token = strtok(NULL, " ");
	}
    printf("Result: %d\n", stack[stackIndex]); // Result adalah hasil jumlah dari element yang ada pada stack
}

void prefixEvaluate(char s[]) {
    int len = strlen(s);
    int stack[100]; // Stack untuk menampung elements
    int stackIndex = -1;
    int i;
    char *rev = (char *)malloc((len + 1) * sizeof(char));

    // Reverse Prefix Expression
    for (i = 0; i < len; i++) {
        rev[i] = s[len - 1 - i];
    }
    rev[len] = '\0';

	char *token = strtok(rev, " ");
	
	while(token!= NULL){
		int c = token[0];
		if (isdigit(c)) {
            stack[++stackIndex] = c - '0'; // Jika input yang masuk adalah operand, maka masukkan ke stack
        } else { //Jika input yang masuk adalah operators
        	// Pop 2 operand
            int op1 = stack[stackIndex--]; 
            int op2 = stack[stackIndex--];
            int result;

            switch (c) {
                case '+':
                    result = op2 + op1;
                    break;
                case '-':
                    result = op2 - op1;
                    break;
                case '*':
                    result = op2 * op1;
                    break;
                case '/':
                    result = op2 / op1;
                    break;
                case '^':
                    result = pow(op2, op1);
                    break;
                default:
                    printf("Invalid operator encountered: %c\n", c);
                    return;
            }
            stack[++stackIndex] = result;
		}
		token = strtok(NULL, " ");
	}
    printf("Result: %d\n", stack[stackIndex]); // Result adalah hasil jumlah dari element yang ada pada stack
    free(rev);
}

int evaluateMenu(){
	char exp[100];
	do{
		printf("====================\n");
		printf("==== EVALUATION ====\n");
		printf("====================\n");
		printf("1. Evaluate Postfix\n");
		printf("2. Evaluate Prefix\n");
		printf("0. Back to main menu\n");
		printf("\n");
	
		do{
			printf("Input number : ");
			scanf("%d", &m);		
		} while(m < 0 && m > 2);
		
		switch(m){
			case 1: 
				printf("========================\n");
				printf("== POSTFIX EVALUATION ==\n");
				printf("========================\n");
				printf("note : Expression must have a space\n");
				printf("input postfix expression: ");
				scanf(" %[^\n]", exp);
				postfixEvaluate(exp);
				printf("\n");
				break;
			case 2:
				printf("=======================\n");
				printf("== PREFIX EVALUATION ==\n");
				printf("=======================\n");
				printf("note : Expression must have a space\n");
				printf("input prefix expression: ");
				scanf(" %[^\n]", exp);
				prefixEvaluate(exp);
				printf("\n");
				break;
			case 0:
				system("@cls||clear");
				return 0;
		}
	} while ( m != 0 );
	return;
}

void displayQuestion(Question q) {
	int i;
    printf("%s\n", q.question); //pertanyaan quiz
}

int checkAnswer(Question q, char* user_answer) {
	int i;
    // Convert user answer and correct answer to uppercase to make the comparison case-insensitive
    for(i = 0; user_answer[i]; i++){
        user_answer[i] = toupper(user_answer[i]);
    }
    for(i = 0; q.correct_answer[i]; i++){
        q.correct_answer[i] = toupper(q.correct_answer[i]);
    }

    if(strcmp(user_answer, q.correct_answer) == 0) {
        printf("Jawaban anda benar!\n");
        printf("\n");
        return 1;
    } else {
        printf("Jawaban anda salah! Jawaban yang benar adalah %s.\n", q.correct_answer);
        printf("\n");
        return 0;
    }
}

void quizGames(struct playerData *scoreData, int totalPlayers) {
	int i, playerScore = 0, score = 0;
    int num_questions = MAX_QUESTIONS;
    char buffer[100];
    
	printf("==================\n");
	printf("====== QUIZ ======\n");
	printf("==================\n");
	printf("\nLet's start the game!\n");
    
    // Masukkan Data ke file (write)
    fp = fopen("Score.txt", "a+");
    if (fp == NULL) {
        printf("file cannot be opened for writing\n");
        exit(1);
    }
    
    // Clear the input buffer to avoid issues with fgets
    while (getchar() != '\n');
	// Input nama
    printf("Your name : ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    strcpy(scoreData[totalPlayers].player, buffer);	

    // Acak (Random) Nomor quiz yang akan keluar
    srand(time(NULL));
    // List pertanyaan dan jawaban
    Question original_questions[MAX_QUESTIONS] = {
        {"Ubah Infix expression berikut menjadi Postfix expression! a+b(c-d)/e*(f-(g/h))", "abcd-e/fgh/-*+"},
        {"Ubah Infix expression berikut menjadi Postfix expression! (F+G)*(L/W)+T", "FG+LW/*T+"},
		{"Ubah Prefix expression berikut menjadi Infix expression! *+AB-CD", "((A+B)*(C-D))"},    
		{"Ubah Infix expression berikut menjadi Prefix expression! D+(C*H+X/(R-V))", "+D+*CH/X-RV"},    
		{"Ubah Postfix expression berikut menjadi Prefix expression! AB+CD-*", "*+AB-CD"},    
		{"Ubah Prefix expression berikut menjadi Postfix expression! -C/V*N^MB*", "CVNMB^*/-"},    
		{"Ubah Postfix expression berikut menjadi Infix expression! SG+AS/*", "((S+G)*(A/S))"},    
		{"Evaluasi Postfix expression berikut! 231*+9-", "-4"},    
		{"Evaluasi Postfix expression berikut! 12*3+45/-", "5"},    
		{"Evaluasi Prefix expression berikut! -*+4325", "9"}
	};

    Question questions[MAX_QUESTIONS]; // Jumlah max pertanyaan : 3
    memcpy(questions, original_questions, sizeof(original_questions)); // Copy the contents of original_questions to questions

    for (i = 0; i < MAX_QUESTIONS; i++) {
    	//Random number of question
        int random_index = rand() % num_questions;
        Question current_question = questions[random_index];
        scoreData[totalPlayers].score = 0;

        displayQuestion(current_question);

        char user_answer[64];
        printf("Masukkan Jawaban: ");
        scanf("%s", user_answer);

        checkAnswer(current_question, user_answer) ? score += 10 : score == 0;
   	
    	printf("Score sementara: %d\n", score);
    	printf("\n");
	
        questions[random_index] = questions[num_questions - 1]; 
        num_questions--; // Tiap pertanyaan yang keluar index nya akan berkurang 1, sehingga pertanyaan akan habis lalu user selesai bermain.
    }
        
	totalScore = scoreData[totalPlayers].score;
    totalScore += score; // score disimpan di totalScore
	// Tampilkan score
	printf("=========================\n");
    printf("== Total score: %d/100 ==\n", totalScore);
    printf("=========================\n");
    // Simpan hasil score quiz ke file Score.txt 
    fprintf(fp, "%s-%d\n", scoreData[totalPlayers].player, totalScore);
    fclose(fp);
}

void sortScoresFromFile(struct playerData *scoreArray, int *totalPlayers) {
    int j, k, i = 0;
    
    fp = fopen("Score.txt", "r"); // Read file
    if (fp == NULL) { // Kalau file tidak ditemukan
        printf("file cannot be opened for reading\n");
        exit(1);
    }
    
    while (fscanf(fp, "%[^-]-%d\n", scoreArray[i].player, &scoreArray[i].totalScore) != EOF) i++; // Menambahkan setiap hasil score quiz
    
    *totalPlayers = i;
    fclose(fp);

    // Sorting scores dari yang tertinggi
    for (j = 0; j < *totalPlayers - 1; j++) {
        for (k = j + 1; k < *totalPlayers; k++) {
            if (scoreArray[j].totalScore < scoreArray[k].totalScore) {
                temp = scoreArray[j];
                scoreArray[j] = scoreArray[k];
                scoreArray[k] = temp;
            }
        }
    }
}

void displayLeaderboard(struct playerData *scoreArray, int totalPlayers) {
	int i;
	printf("=========================\n");
	printf("====== LEADERBOARD ======\n");
	printf("=========================\n");
    for (i = 0; i < totalPlayers; i++) {
        printf("%d. %s - %d\n", i + 1, scoreArray[i].player, scoreArray[i].totalScore); // Display List nama dan score setelah mengerjakan quiz
    }
}

int quizMenu(){
    int totalPlayers = 1;
	struct playerData scoreData[100];
	
    fp = fopen("Score.txt", "a+"); // Display dan Read file
    if(fp == NULL){ // Jika file tidak ditemukan
        printf("file cannot be opened for writing\n");
        exit(1);
    }
		
	do{
		printf("=================\n");
		printf("=== QUIZ MENU ===\n");
		printf("=================\n");
		printf("1. Quiz\n");
		printf("2. Leaderboard\n");
		printf("0. Back to main menu\n");
		printf("\n");
	
		do{
			printf("Input number : ");
			scanf("%d", &m);		
		} while(m < 0 && m > 2);
        
		switch(m){
			case 1:
				system("@cls||clear");
                quizGames(scoreData, totalPlayers);
                totalPlayers++;
				printf("\n");
				break;
			case 2:
				system("@cls||clear");
				sortScoresFromFile(scoreData, &totalPlayers);
				displayLeaderboard(scoreData, totalPlayers);
				printf("\n");
				break;
			case 0:
				fclose(fp);
				system("@cls||clear");
				return 0;
		}
	} while ( m != 0 );
	return;
}

int main() {
	int m;
	do{
		m = menu();
		switch(m){
			case 1:
				converterMenu();
				printf("\n");
				break;
			case 2:
				evaluateMenu();
				printf("\n");
				break;
			case 3:
				quizMenu();
				printf("\n");
				break;
			default:
				printf("Exit");
				break;
		}
	} while(m != 0);
	return 0;
}

