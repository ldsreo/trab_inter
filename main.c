// TRABALHO INTERMEDIO
//
// 89190 Leonardo da Silva Rosa e Oliveira
//
//
// Compile with:
// gcc main.c -g -I/usr/include/SDL2 -D_REENTRANT -Wall -pedantic -std=c99 -L/usr/lib/i386-linux-gnu -lm -lSDL2 -lSDL2_gfx -o inter

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

void configurar(int configs[6], float *pdl);
void salvar(int pts);
void nova_janela(int configs[6], int size[5], float dl);
void desenhar_campo(int configs[6], float dl, int offset_top, char *campo);
void desenhar_hud(int *size_x, int bolha, float dl, int *random_prox);
void desenhar_menu(int size[5], int bolha, int pts);
void atirar_bolha(int size[5], int bolha, int random);

SDL_Window* g_pWindow = 0;
SDL_Renderer* g_pRenderer = 0;

int main(void)
{
	int configs[6];	
	int size[5];						/* window size {x, y, campo_y, offset_top, offset_bottom}*/
	int random_color[2] = {rand() % 9, 0};	/* atual, proxima */
	int pontuacao = 0;

	// LER CONFIG.TXT
	configurar(configs, &dl);

	// CRIAR JANELA C/ SDL
	nova_janela(configs, &size[0], dl);

	char campo_bolha[size[0]][size[2]];	/* usar 'char' p/ ocupar menos memoria */
	for (int i = 0; i < configs[0]*configs[1]; i++) {
		*(&campo_bolha[0][0] + i) = -1;
	}

	desenhar_campo(configs, dl, size[3], &campo_bolha[0][0]);
	desenhar_hud(&size[0], configs[2], dl, &random_color[1]);
	desenhar_menu(&size[0], configs[2], pontuacao);

	// Rodar jogo

	// Fim de jogo

	// Pedir nome e salvar pts em resultados.txt
	salvar(pontuacao);

	// Oferecer jogo novamente

	return 0;
}

void configurar(int configs[6], float *pdl)
{
	/* dimensao x da janela em bolhas 		*/
	/* dimensao y da janela em bolhas 		*/
	/* diametro da bolha	 			*/
	/* NULL							*/
	/* numero inicial de linhas 			*/
	/* criar nova linha a cada 'n' rodadas	*/

	char ch, linha[100];	
	int i = 0;
	FILE *file_in = fopen("config.txt","r");

	if (file_in == NULL) {
		printf("\tERRO: configurar\n");
		printf("\t\tNao foi possivel abrir o arquivo \"config.txt\"\n");
		return;
	}

	do {
		ch = fgetc(file_in);
		if (ch != '\n') {
			if ((ch > 47) && (ch < 58)) {
				ungetc(ch, file_in);
				if (i == 3)
					fscanf(file_in, "%f", pdl);
				else
					fscanf(file_in, "%d", &configs[i]);
				i++;
			}
			else if (ch != ' ') {
				fgets(linha, 100, file_in);
			}
		}
	} while (ch != EOF)

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

void nova_janela(int configs[6], int size[5], float dl)
{
	size[0] = configs[0] * configs[2] * (1 + dl);		/* x			*/
	size[4] = 2 * configs[2] * (1+dl);					/* offset_bottom */ 
	if (size[0] < 255)								/* offset_top	*/
		size[3] = 45;
	else
		size[3] = 25;
	size[2] = (configs[1] + 2) * configs[2] * (1 + dl);	/* campo_y	*/
	size[1] = size[2] + size[3] + size[4]; 				/* y			*/
	
	// initialize SDL
	if(SDL_Init(SDL_INIT_EVERYTHING) >= 0) {
		// if succeeded create our window
		g_pWindow = SDL_CreateWindow("Trabalho Intermedio", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, size[0], size[1], SDL_WINDOW_SHOWN);

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

void desenhar_campo(int configs[6], float dl, int offset_top, char *campo)
{
	int x, y, r = 0.5 * configs[2] * dl;
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

	for (int i = 0; i < configs[0]; i++) {
		for (int j = 0; j < configs[4]; j++) {
			x = (i + 0.5) * configs[2] * (1 + dl);
			y = (j + 0.5) * configs[2] * (1 + dl) + offset_top;
			random = rand() % 9;
			filledCircleRGBA(g_pRenderer, x, y, configs[2]/2, color[random][0], color[random][1], color[random][2], 255);
			for (int dx = -r; dx <= r; dx++) {
				for (int dy = -r; dy <= r; dy++) {
					if ((pow(dx,2) + pow(dy,2)) <= pow(r,2))	/* se dentro da bolha */
						*(campo + (x+dx) + (y+dy)) = random;	/* definir cor no campo */
				}
			}
		}
	}
	// show the window
 	SDL_RenderPresent(g_pRenderer);
}

void desenhar_hud(int *size_y, int bolha, float dl, int *random_prox)
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

	// rect
	s = bolha * (1 + dl) + 1;
	x = 0;
	y = *size_y - s;	/* size_y - side */
	rectangleRGBA(g_pRenderer, x, y, x+s, y+s, 0, 0, 0, 255);

	// prox bolha
	*random_prox = rand() % 9;					/* definir novo random */
	x = 0.5 * bolha * (1 + dl);
	y = *size_y - (0.5 * bolha * (1 + dl));		/* size_y - bolha/2 */
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

void atirar_bolha(int size[5], int bolha, int random)
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

	// current bolha
	x = size[0]) / 2;						/* size_x / 2 		*/
	y = size[1] - (0.5 * bolha * (1 + dl));		/* size_y - bolha/2 */
	filledCircleRGBA(g_pRenderer, x, y, bolha/2, color[random][0], color[random][1], color[random][2], 255);
	
	return;
}