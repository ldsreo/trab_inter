// TRABALHO INTERMEDIO
//
// Compile with:
// gcc main.c -g -I/usr/include/SDL2 -D_REENTRANT -Wall -pedantic -std=c99 -L/usr/lib/i386-linux-gnu -lm -lSDL2 -lSDL2_gfx -o inter

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

void configurar(int *pjanela_x, int *pbolha, float *pdl, int *pnum_linha_0, int *pnova_linha);
void salvar(int pts);
void nova_janela(int *janela_x, int *size_x, int bolha, int num_linha_0, float dl);
void desenhar_campo(int janela_x, int bolha, float dl, int num_linha_0, int *campo_00);
void desenhar_hud(int *size_x, int bolha, float dl, int *random_prox);
void desenhar_menu(int *size_x, int bolha, int pts);

SDL_Window* g_pWindow = 0;
SDL_Renderer* g_pRenderer = 0;

int main(void)
{
	int janela[2];						/* dimensoes da janela (x,y) em bolhas 					*/
	int bolha;						/* dimensao da bolha (d) 							*/
	float dl;							/* espaco para colisao, em % de 'd'	 				*/
	int num_linha_0;					/* numero inicial de linhas 							*/
	int nova_linha;					/* criar nova linha a cada 'n' rodadas 					*/
	int size[5];						/* window size {x, y, campo_y, offset_top, offset_bottom}*/
	int random_color[2] = {rand() % 9, 0};	/* atual, proxima */
	int pontuacao = 0;
	
	// LER CONFIG.TXT
	configurar(&janela[0], &bolha, &dl, &num_linha_0, &nova_linha);
	int campo_bolha[janela[0]][janela[1]];
	for (int i = 0; i < janela[0]*janela[1]; i++) {
		*(&campo_bolha[0][0] + i) = -1;
	}

	// CRIAR JANELA C/ SDL
	nova_janela(&janela[0], &size[0], bolha, num_linha_0, dl);
printf("1\n");
	desenhar_campo(janela[0], bolha, dl, num_linha_0, &campo_bolha[0][0]);
printf("2\n");
	desenhar_hud(&size[0], bolha, dl, &random_color[1]);
printf("3\n");
	desenhar_menu(&size[0], bolha, pontuacao);
	// Rodar jogo

	// Fim de jogo

	// Pedir nome e salvar pts em resultados.txt
	salvar(pontuacao);

	// Oferecer jogo novamente

	return 0;
}

void configurar(int *pjanela_x, int *pbolha, float *pdl, int *pnum_linha_0, int *pnova_linha)
{
	char ch;
	char linha[100];
	void *pointers[6] = {pjanela_x, pjanela_x + 1, pbolha, pdl, pnum_linha_0, pnova_linha};	
	int i = 0;

	FILE *file_in = fopen("config.txt","r");

	if (file_in == NULL) {
		printf("\tERRO: configurar\n");
		printf("\t\tNao foi possivel abrir o arquivo \"config.txt\"\n");
		return;
	}

	ch = fgetc(file_in);
	while (ch != EOF) {
		if (ch != '\n') {
			if ((ch > 47) && (ch < 58)) {
				ungetc(ch, file_in);
				if (i == 3)
					fscanf(file_in, "%f", pointers[i]);
				else
					fscanf(file_in, "%d", pointers[i]);
				i++;
			}
			else if (ch != ' ') {
				fgets(linha, 100, file_in);
			}
		}
		ch = fgetc(file_in);
	}
	*pdl *= 0.01;
	fclose(file_in);

	return;
}

void salvar(int pts)
{
	char nome[16];

	printf("\tNome: ");
	scanf("%15s", &nome);

	FILE *file_out = fopen("resultados.txt","a+");
	fprintf(file_out, "%s: \t %d \n", nome, pts);
	fclose(file_out);

	return;
}

void nova_janela(int *janela_x, int *size_x, int bolha, int num_linha_0, float dl)
{
	*(size_x+4) = 2 * bolha * (1+dl);					/* offset_bottom */ 
	*(size_x+3) = 50;								/* offset_top	*/
	*(size_x+2) = (*(janela_x+1) + 2) * bolha * (1 + dl);	/* campo_y	*/
	*(size_x+1) = *(size_x+2) + *(size_x+3) + *(size_x+4); /* y			*/
	*size_x = *janela_x * bolha * (1 + dl);				/* x			*/
	
	// initialize SDL
	if(SDL_Init(SDL_INIT_EVERYTHING) >= 0) {
		// if succeeded create our window
		g_pWindow = SDL_CreateWindow("Trabalho Intermedio",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, *size_x, *(size_x+1), SDL_WINDOW_SHOWN);

		// if the window creation succeeded create our renderer
		if(g_pWindow != 0) {
			g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, 0);
		}
	}
	else {
		printf("\tERRO: nova_janela\n");
		printf("\t\tSDL nao conseguiu criar a janela.\n");
		return; // sdl could not initialize
	}
	// draw the window
	SDL_SetRenderDrawColor(g_pRenderer, 220, 220, 220, 1);
	// clear the window to white
	SDL_RenderClear(g_pRenderer);

 	// show the window
 	SDL_RenderPresent(g_pRenderer);

 	return;
}

void desenhar_campo(int janela_x, int bolha, float dl, int num_linha_0, int *campo_00)
{
	int x, y;
	int color[9][3] =  {{255, 0, 0},		/* vermelho */
					{128, 0, 128},		/* roxo */
					{0, 0, 255},		/* azul */
					{0, 255, 255},		/* cyan */
					{0, 128, 0},		/* verde */
					{255, 255, 0},		/* amarelo */
					{128, 0, 0},		/* castanho */
					{0, 0, 0},		/* preto */
					{255, 255, 255}};	/* branco */
	int random;

	for (int i = 0; i < janela_x; i++) {
		for (int j = 0; j < num_linha_0; j++) {
			x = (i + 0.5) * bolha * (1 + dl);
			y = (j + 0.5) * bolha * (1 + dl);
			random = rand() % 9;
			filledCircleRGBA(g_pRenderer, x, y+50, bolha/2, color[random][0], color[random][1], color[random][2], 255);
			*(campo_00 + i*j) = random;
		}
	}
	// show the window
 	SDL_RenderPresent(g_pRenderer);
}

void desenhar_hud(int *size_x, int bolha, float dl, int *random_prox)
{
	int x, y, s;
	int color[9][3] =  {{255, 0, 0},		/* vermelho */
					{128, 0, 128},		/* roxo */
					{0, 0, 255},		/* azul */
					{0, 255, 255},		/* cyan */
					{0, 128, 0},		/* verde */
					{255, 255, 0},		/* amarelo */
					{128, 0, 0},		/* castanho */
					{0, 0, 0},		/* preto */
					{255, 255, 255}};	/* branco */

	// current bolha
//	x = *(size_x) / 2;						/* size_x / 2 		*/
//	y = *(size_x+1) - (0.5 * bolha * (1 + dl));	/* size_y - bolha/2 */
//	filledCircleRGBA(g_pRenderer, x, y, bolha/2, color[*random][0], color[*random][1], color[*random][2], 255);

	// rect
	s = bolha * (1 + dl) + 1;
	x = 0;
	y = *(size_x+1) - s;	/* size_y - side */
	rectangleRGBA(g_pRenderer, x, y, x+s, y+s, 0, 0, 0, 255);

	// prox bolha
	*random_prox = rand() % 9;					/* definir novo random */
	x = 0.5 * bolha * (1 + dl);
	y = *(size_x+1) - (0.5 * bolha * (1 + dl));		/* size_y - bolha/2 */
	filledCircleRGBA(g_pRenderer, x, y, bolha/2, color[*random_prox][0], color[*random_prox][1], color[*random_prox][2], 255);

	// show the window
 	SDL_RenderPresent(g_pRenderer);
	return;
}

void desenhar_menu(int *size_x, int bolha, int pts)
{
	int x, y, w, h;
	char pontos[14] = "Pontos:      \0";
	for (int i = 1; pow(10,i+1) < pts; i++) {
		pontos[13-i] = pts % (int)pow(10,i);
	}
	gfxPrimitivesSetFontRotation(0);
	// caixa_novo
	x = 5;
	y = 5;
	w = 80;
	h = 15;
	rectangleRGBA(g_pRenderer, x, y, x+w, y+h, 50, 50, 50, 255);

	// caixa_fim
	x = 90;
	w = 35;
	rectangleRGBA(g_pRenderer, x, y, x+w, y+h, 50, 50, 50, 255);

	// texto_novo
	x = 10;
	y = 10;
 	stringRGBA(g_pRenderer, x, y, "Novo Jogo", 0, 0, 0, 255);

	// texto_fim
	x = 95;
 	stringRGBA(g_pRenderer, x, y, "Fim", 0, 0, 0, 255);

	// caixa_pts
	if (*size_x < 255) {	/* pos. = embaixo */
		x = 5;
		y = 25;
	}
	else {					/* pos. = ao lado */
		x = *size_x - 125;
		y = 5;
	}	
	w = 120;
	h = 15;
	rectangleRGBA(g_pRenderer, x, y, x+w, y+h, 50, 50, 50, 255);

	// texto_pts
	x += 5;
	y += 5;
 	stringRGBA(g_pRenderer, x, y, pontos, 0, 0, 0, 255);

	// show the window
 	SDL_RenderPresent(g_pRenderer);
	return;
}