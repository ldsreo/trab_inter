// TRABALHO INTERMEDIO
//
// 89190 Leonardo da Silva Rosa e Oliveira
//
//
// Compile with:
// gcc main.c -g -I/usr/include/SDL2 -D_REENTRANT -Wall -pedantic -std=c99 -L/usr/lib/i386-linux-gnu -lm -lSDL2 -lSDL2_gfx -o inter


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

void configurar(int configs[6], float *pdl);
void salvar(int pts);
void nova_janela(int configs[6], int size[5], float dl);
void desenhar_campo(int configs[6], float dl, int offset_top, int size_campo_y, int campo[][1500][3]);
void desenhar_hud(int size[5], int bolha, float dl, int random[2]);
void desenhar_menu(int *size_x, int pts);
void atirar_bolha(int size[5], int bolha, float dl, int random, float *angle);
int mover_bolha(int size[5], float bolha[4], float dl, float angle, int campo[][1500][3]);
int verificar_colisao(int size[5], float bolha[4], float dl, int campo[][1500][3], int *pop);
int pop_bolhas(float bolha[4], float dl, int campo[][1500][3], int direcao, int offset_top);
void colar_bolha(float bolha[4], float dl, int colisao, int campo[][1500][3]);
void update_campo(int pix_x, int pix_y, int cor, int r, int campo[][1500][3]);

SDL_Window* g_pWindow = 0;
SDL_Renderer* g_pRenderer = 0;

int main(void)
{
	int configs[6];									/* << ver definicao em configurar() >> */
	float dl, angle;
	int size[5];									/* window size {x, y, campo_y, offset_top, offset_bottom}*/
	int random_color[2] = {rand() % 9, 0};			/* atual, proxima */
	float bolha[4];									/* info da bolha {pos_x, pos_y, raio, cor} */
	int fim = 0, pontuacao = 0;

	// LER CONFIG.TXT
	configurar(configs, &dl);

	// CRIAR JANELA C/ SDL
	nova_janela(configs, size, dl);

	int campo_bolha[size[2]][1500][3];				/* usar 'char' p/ ocupar menos memoria */
	//memset(campo_bolha, -1, size[2]*1500*3*sizeof(char));
	for (int i = 0; i < size[2]; i++)
		for (int j = 0; j < 1500; j++)
			for (int k = 0; k < 3; k++)
			campo_bolha[i][j][k] = -1;
	
	desenhar_campo(configs, dl, size[3], size[2], campo_bolha);
	desenhar_hud(size, configs[2], dl, random_color);
	desenhar_menu(&size[0], pontuacao);

	// RODAR JOGO
	for (int i = 0; i < 5; i++) {
		atirar_bolha(size, configs[2], dl, random_color[0], &angle);
		bolha[0] = size[0] / 2;									/* x0 */
		bolha[1] = size[1] - (configs[2] * (1 + dl) + 1);		/* y0 */
		bolha[2] = configs[2];									/* raio */
		bolha[3] = random_color[0];								/* cor */
		pontuacao += mover_bolha(size, bolha, dl, angle, campo_bolha);
		random_color[0] = random_color[1];
		desenhar_hud(size, configs[2], dl, random_color);
		desenhar_menu(&size[0], pontuacao);
	}

	// Fim de jogo

	// Pedir nome e salvar pts em resultados.txt
	salvar(pontuacao);

	// Oferecer jogo novamente

	return 0;
}

void configurar(int configs[6], float *pdl)
{
	/* configs[6] 										*/
	/* 		[0] = dimensao x da janela em bolhas 		*/
	/* 		[1] = dimensao y da janela em bolhas 		*/
	/* 		[2] = raio da bolha	 					*/
	/* 		[3] = NULL									*/
	/* 		[4] = numero inicial de linhas 				*/
	/* 		[5] = criar nova linha a cada 'n' rodadas	*/

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
	} while (ch != EOF);

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
	size[0] = configs[0] * 2 * configs[2] * (1 + dl);			/* x				*/
	size[4] = 2 * configs[2] * (1+dl);							/* offset_bottom 	*/ 
	if (size[0] < 255)										/* offset_top		*/
		size[3] = 45;
	else
		size[3] = 25;
	size[2] = configs[1] * 2 * configs[2] * (1 + dl);		/* campo_y			*/
	size[1] = size[2] + size[3] + size[4]; 					/* y				*/
	
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

void desenhar_campo(int configs[6], float dl, int offset_top, int size_campo_y, int campo[][1500][3])
{
	int x, y, r = configs[2] * (1+dl);
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

	for (int i = 0; i < configs[0]; i++)
	{
		for (int j = 0; j < configs[1]; j++)
		{
			x = (2*i + 1) * configs[2] * (1 + dl);
			y = (2*j + 1) * configs[2] * (1 + dl);
			if (j < configs[4])
			{
				random = rand() % 9;
				filledCircleRGBA(g_pRenderer, x, y+offset_top, configs[2], color[random][0], color[random][1], color[random][2], 255);
			}
			for (int dx = -r; dx <= r; dx++)
			{
				for (int dy = -r; dy <= r; dy++)
				{
					if (y+dy >= size_campo_y)
						break;
					campo[y+dy][x+dx][1] = x;					/* definir centro (x) */
					campo[y+dy][x+dx][2] = y+offset_top;		/* definir centro (y) */
					if ((j < configs[4]) && ((pow(dx,2) + pow(dy,2)) <= pow(r,2)))	/* se dentro da bolha */
					{
						campo[y+dy][x+dx][0] = random;			/* definir cor no campo */
					}
				}
			}

		}
	}
	// show the window
 	SDL_RenderPresent(g_pRenderer);
}

void desenhar_hud(int size[5], int bolha, float dl, int random[2])
{
	int x, y, s;
	int color[9][3] =	{{255, 0, 0},		/* vermelho */
						{128, 0, 128},		/* roxo */
						{0, 0, 255},		/* azul */
						{0, 255, 255},		/* cyan */
						{0, 128, 0},		/* verde */
						{255, 255, 0},		/* amarelo */
						{128, 0, 0},		/* castanho */
						{0, 0, 0},			/* preto */
						{255, 255, 255}};	/* branco */
	random[1] = rand() % 9;

	// rect
	s = 2 * bolha * (1 + dl) + 3;
	x = 0;
	y = size[1] - s;	/* size_y - side */
	rectangleRGBA(g_pRenderer, x, y, x+s, y+s, 0, 0, 0, 255);

	// prox bolha
	x = 1 + bolha * (1 + dl);
	y = size[1] - (bolha * (1 + dl) + 1);		/* size_y - bolha/2 */
	filledCircleRGBA(g_pRenderer, x, y, bolha, color[random[1]][0], color[random[1]][1], color[random[1]][2], 255);

	// current bolha
	x = size[0] / 2;							/* size_x / 2 		*/
	y = size[1] - (bolha * (1 + dl) + 1);		/* size_y - bolha/2 */
	filledCircleRGBA(g_pRenderer, x, y, bolha, color[random[0]][0], color[random[0]][1], color[random[0]][2], 255);

	// show the window
 	SDL_RenderPresent(g_pRenderer);
	return;
}

void desenhar_menu(int *size_x, int pts)
{
	int x, y, w, h;
	char pontos[50] = "Pontos: ";
	char pts_str[50];
	sprintf(pts_str, "%5d", pts);
	strcat(pontos, pts_str);
	/*
	for (int i = 1; pow(10,i+1) < pts; i++) {
		pontos[13-i] = pts % (int)pow(10,i);
	}
	*/
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

void atirar_bolha(int size[5], int bolha, float dl, int random, float *angle)
{
	int xl, yl, xm, ym, xt1, yt1, xt2, yt2;
	int x0 = size[0] / 2;
	int y0 = size[1] - (bolha * (1 + dl) + 1);
	int atirar = 0;
	int color[9][3] =  {{255, 0, 0},		/* vermelho */
					{128, 0, 128},		/* roxo */
					{0, 0, 255},		/* azul */
					{0, 255, 255},		/* cyan */
					{0, 128, 0},		/* verde */
					{255, 255, 0},		/* amarelo */
					{128, 0, 0},		/* castanho */
					{0, 0, 0},		/* preto */
					{255, 255, 255}};	/* branco */
	SDL_Event event;

	while (atirar != 1) {
		SDL_WaitEvent(&event);
		SDL_GetMouseState(&xm, &ym);
		*angle = atan2(ym-y0, xm-x0);
		switch (event.type) {
			case (SDL_MOUSEMOTION) :		/* ajustar direcao da seta */
				lineRGBA(g_pRenderer, x0, y0, xl, yl, 220, 220, 220, 255);
				filledCircleRGBA(g_pRenderer, x0, y0, bolha, color[random][0], color[random][1], color[random][2], 255);
				xl = cos(*angle) * 4 * bolha + x0;
				yl = sin(*angle) * 4 * bolha + y0;
				lineRGBA(g_pRenderer, x0, y0, xl, yl, 0, 0, 0, 255);
				xt1 = (xl - bolha) * cos(*angle) - (yl + bolha) * sin(*angle);
				yt1 = (xl - bolha) * sin(*angle) + (yl + bolha) * cos(*angle);
				xt2 = (xl - bolha) * cos(*angle) - (yl - bolha) * sin(*angle);
				yt2 = (xl - bolha) * sin(*angle) + (yl - bolha) * cos(*angle);
				//filledTrigonRGBA(g_pRenderer, x, y, xt1, yt1, xt2, yt2, 0, 0, 0, 255);
				SDL_RenderPresent(g_pRenderer);
				SDL_Delay(25);
				break;
			case (SDL_MOUSEBUTTONDOWN) :	/* atirar */
				if (*angle < 0) {
					lineRGBA(g_pRenderer, x0, y0, xl, yl, 220, 220, 220, 255);	/* limpar seta */
					atirar = 1;
				}
				break;
		}
	}
	
	
	return;
}

int mover_bolha(int size[5], float bolha[4], float dl, float angle, int campo[][1500][3])
{
	int pix_x = (int)bolha[0];
	int pix_y = (int)bolha[1];
	int raio = (int)bolha[2];
	int color[9][3] =	{{255, 0, 0},		/* vermelho */
						{128, 0, 128},		/* roxo */
						{0, 0, 255},		/* azul */
						{0, 255, 255},		/* cyan */
						{0, 128, 0},		/* verde */
						{255, 255, 0},		/* amarelo */
						{128, 0, 0},		/* castanho */
						{0, 0, 0},			/* preto */
						{255, 255, 255}};	/* branco */
	int cor = (int)bolha[3];
	int colisao = 0, pop = 0, pts = 0;

	while (colisao == 0) {
		filledCircleRGBA(g_pRenderer, pix_x, pix_y, raio, 220, 220, 220, 255);
		bolha[0] += cos(angle);
		bolha[1] += sin(angle);
		pix_x = (int)bolha[0];
		pix_y = (int)bolha[1];
		colisao = verificar_colisao(size, bolha, dl, campo, &pop);
		filledCircleRGBA(g_pRenderer, pix_x, pix_y, raio, color[cor][0], color[cor][1], color[cor][2], 255);
		SDL_RenderPresent(g_pRenderer);
		SDL_Delay((int)bolha[2]/5);
	}
	filledCircleRGBA(g_pRenderer, pix_x, pix_y, raio, 220, 220, 220, 255);
	SDL_RenderPresent(g_pRenderer);
	printf("%d %d\n", campo[pix_y][pix_x][1], campo[pix_y][pix_x][2]);
	if (colisao > 1) {
		colar_bolha(bolha, dl, colisao, campo);
		pix_x = bolha[0], pix_y = bolha[1];
		if (pop != 0)
		{
			pop_bolhas(bolha, dl, campo, colisao, size[3]);
		}
		else
		{
			update_campo(pix_x, pix_y-size[3], cor, raio*(1+dl), campo);
			filledCircleRGBA(g_pRenderer, pix_x, pix_y, bolha[2], color[cor][0], color[cor][1], color[cor][2], 255);
		}
		SDL_RenderPresent(g_pRenderer);
	}
	return pts;
}

int verificar_colisao(int size[5], float bolha[4], float dl, int campo[][1500][3], int *pop)
{
	float pos_x = bolha[0];
	float pos_y = bolha[1];
	float raio = bolha[2];
	char cor_bolha = (int)bolha[3];
	int max_x = size[0];
	int min_y = size[3];
	int x, y;

	if ((pos_x - raio*(1+dl) <= 0) || (pos_x + raio*(1+dl) >= max_x) || (pos_y - raio*(1+dl) <= min_y))
		return -1;

	for (float theta = 0; theta > -M_PI; theta -= M_PI/60)
	{
		x = pos_x + raio*(1+dl)*cos(theta);
		y = pos_y + raio*(1+dl)*sin(theta) - size[3];
		if (y < size[2])
		{
			if (campo[y][x][0] == cor_bolha)
				*pop = 1;
			if (campo[y][x][0] != -1)
			{									/* nova posicao da bolha: 	*/
				if (theta > -M_PI/4.0)
					return 9;					/* 		9 hr 				*/
				else if (theta < -3*M_PI/4.0)
					return 3;					/* 		3 hr 				*/
				else if (theta > -M_PI/2.0)
					return 7;					/*		6 hr from left		*/
				else
					return 5;					/*		6 hr from right		*/
			}
		}
	}

	return 0;
}

int pop_bolhas(float bolha[4], float dl, int campo[][1500][3], int direcao, int offset_top)
{
	int raio = bolha[2];
	if (direcao == 3)
		bolha[0] -= 2 * raio * (1+dl);
	else if (direcao == 9)
		bolha[0] += 2 * raio * (1+dl);
	else if (direcao != 0)
		bolha[1] -= 2 * raio * (1+dl);
	int pix_x = bolha[0];
	int pix_y = bolha[1];
	int x, y, pts = 1;

	filledCircleRGBA(g_pRenderer, pix_x, pix_y, raio, 220, 220, 220, 255);
	SDL_RenderPresent(g_pRenderer);
	SDL_Delay((int)bolha[2]/5);

	update_campo(pix_x, pix_y-offset_top, -1, raio*(1+dl), campo);

	for (int i = 0; i < 3; i++)
	{
		x = pix_x + raio*(1+dl)*cos(-M_PI*i/2);
		y = pix_y + raio*(1+dl)*sin(-M_PI*i/2) - offset_top;
		if ((x > 0) && (y > 0))
		{
			if (campo[y][x][0] == bolha[3])
			{
				bolha[0] = x;
				bolha[1] = y + offset_top;
				pts += pop_bolhas(bolha, dl, campo, 0, offset_top);
			}
		}

	}
	filledCircleRGBA(g_pRenderer, pix_x, pix_y, raio, 220, 220, 220, 255);
	return pts;
}

void colar_bolha(float bolha[4], float dl, int colisao, int campo[][1500][3])
{
	int x = bolha[0];
	int y = bolha[1];
	int r = bolha[2] * (1+dl);

	switch (colisao)
	{
		case 9:
			bolha[0] = campo[y-r][x-r][1];
			bolha[1] = campo[y-r][x-r][2];
			return;
		case 7: case 5:
			bolha[0] = campo[y][x][1];
			bolha[1] = campo[y][x][2];
			return;
		case 3:
			bolha[0] = campo[y-r][x+r][1];
			bolha[1] = campo[y-r][x+r][2];
			return;
	}
}

void update_campo(int pix_x, int pix_y, int cor, int r, int campo[][1500][3])
{
	for (int dx = -r; dx <= r; dx++)
	{
		for (int dy = -r; dy <= r; dy++)
		{
			if ((pow(dx,2) + pow(dy,2)) <= pow(r,2))	/* se dentro da bolha */
			{
				campo[pix_y+dy][pix_x+dx][0] = cor;		/* definir cor no campo */
			}
		}
	}
	return;
}