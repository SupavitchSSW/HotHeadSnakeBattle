#include "stdafx.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <vector>
#include "time.h"
using namespace std;

#define MAINMENU 1
#define MAPSELECTMENU 2
#define MAP1 3
#define MAP2 4
#define MAP3 5
#define MAP_WTF 6
#define SCORERESULTSINGLE 7
#define SCORERESULTMUTI 8
#define EXIT 9
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define P1WIN 1
#define P2WIN 2
#define DRAW 3
#define LOSE 4

int Now_screen = MAINMENU, Now_select = 1, Picred_menu_posX = 350, Picred_menu_direction = 0, Picblue_menu_posX = 1000, Picblue_menu_direction = 1;
int Numplayer = 2, snake1P_direction = DOWN, snake2P_direction = UP;
bool snake1P_canturn = true,snake1P_eating = false , snake2P_canturn = true ,snake2P_eating = false , foodspawn = true,bombspawn = true;
int pause = 0, maparray[96][54] = { 0 }, result = 0 , food_spawn_time;
int P1score = 0, P2score = 0 , stage = 1 , gamespeed = 50 , nloop = 0;
int start_time, bomb_spawn_time , r = 0;

sf::RenderWindow window(sf::VideoMode(1920, 1080), "Hot Head Snake Battle", sf::Style::Fullscreen, sf::ContextSettings{ 24,8,4,0,3,0,false });
sf::Event event;
sf::Clock start_clock,food_clock,bomb_clock;
sf::Sound sound_bomb;
sf::Sound sound_beep;



struct obj {
	int x, y , time;
	int explode = false;
}savebox,temp,oo_obstacle;

vector <obj> wall,snake1P,snake2P,food,bomb,obstacle,map2,map3,wtf_map;
// food vector x , y is base on 1/20

void del_food(int x,int y)
{
	for (int i = 0;i < food.size();i++) {
		if (food[i].x == x && food[i].y == y) {
			maparray[food[i].x][food[i].y] = 0;
			food.erase(food.begin() + i);
		}
	}
}

void del_bomb(int x, int y)
{
	for (int i = 0;i < bomb.size();i++) {
		if (bomb[i].x == x && bomb[i].y == y) {
			bomb.erase(bomb.begin() + i);
		}
	}
}

void clear_map()
{
	for (int i = 0;i < 96;i++) {
		for (int j = 0;j < 54;j++) {
			maparray[i][j] = 0;
		}
	}
	snake1P.clear();
	snake2P.clear();
	food.clear();
	bomb.clear();
}

void check_result()
{
	switch (result)
	{
	case P1WIN: {
		Now_screen = SCORERESULTMUTI;
		break;
	}
	case P2WIN: {
		Now_screen = SCORERESULTMUTI;
		break;
	}
	case DRAW: {
		Now_screen = SCORERESULTMUTI;
		break;
	}
	case LOSE: {
		Now_screen = SCORERESULTSINGLE;
		break;
	}
	}
	Now_select = 1;
}

void check_maparray()
{
	//check map array
	printf("\n");
	for (int i = 0;i < 54;i++) {
		for (int j = 0;j < 96;j++) {
			printf("%d", maparray[j][i]);
		}
		printf("\n");
	}
}

int check_coliation(int x , int y , int p)
{
	switch (maparray[x/20][y/20])
	{
	case 1: {
		if (Numplayer == 1) {
			result = LOSE;
			return 1;
		}
		if (p == 1) {
			result = P2WIN;
			return 1;
		}
		if (p == 2) {
			result = P1WIN;
			return 1;
		}
		break;
	}
	case 2: {
		sound_beep.play();
		if (p == 1) {
			del_food(x / 20, y / 20);
			snake1P_eating = true;
			return 2;
		}
		if (p == 2) {
			del_food(x / 20, y / 20);
			snake2P_eating = true;
			return 2;
		}
		break;
	}
	case 3: {
		result = DRAW;
		return 1;
	}
	default: return 0;
	}
	/*if (maparray[x][y] == 1) {
		if (Numplayer == 1) {
			result = LOSE;
			return 1;
		}
		else {
			result = P2WIN;
			return 1;
		}
	}
	else if (maparray[snake1P[0].x / 20][snake1P[0].y / 20 - 1] == 2) {
		snake1P_eating = true;
		del_food(snake1P[0].x / 20, snake1P[0].y / 20 - 1);
	}
	else if (maparray[snake1P[0].x / 20][snake1P[0].y / 20 - 1] == 3) {
		result = DRAW;
		break;
	}*/
	
}

void check_body(int x, int y, int p)
{
	if (p == 1) {
		if (snake1P_eating == true) {
			obj oo;
			oo.x = x;
			oo.y = y;
			snake1P.push_back(oo);
			P1score++;
			snake1P_eating = false;
		}
		else {
			maparray[x/20][y/20] = 0;
		}
	}
	else {
		if (snake2P_eating == true) {
			obj oo;
			oo.x = savebox.x;
			oo.y = savebox.y;
			snake2P.push_back(oo);
			P2score++;
			snake2P_eating = false;
		}
		else {
			maparray[savebox.x / 20][savebox.y / 20] = 0;
		}
	}
}

void create_wall()
{
	obj oo;
	//top wall
	for (int i = 0; i < 76; i++) {
		oo.x = 200 + (i * 20);
		oo.y = 100;
		maparray[oo.x / 20][oo.y / 20] = 1;
		wall.push_back(oo);
	}
	//botton wall
	for (int i = 0; i < 76; i++) {
		oo.x = 200 + (i * 20);
		oo.y = 980;
		maparray[oo.x / 20][oo.y / 20] = 1;
		wall.push_back(oo);
	}
	//left wall
	for (int i = 0; i < 44; i++) {
		oo.x = 200;
		oo.y = 100+(i*20);
		maparray[oo.x / 20][oo.y / 20] = 1;
		wall.push_back(oo);
	}
	//right wall
	for (int i = 0; i < 44; i++) {
		oo.x = 1700;
		oo.y = 100+(i*20);
		maparray[oo.x / 20][oo.y / 20] = 1;
		wall.push_back(oo);
	}
}

void create_map2()
{
	obj oo;
	for (int i = 0;i < 15;i++) {
		oo.x = 32*20;
		oo.y = (i+5)*20;
		maparray[oo.x / 20][oo.y / 20] = 1;
		map2.push_back(oo);
	}

	for (int i = 0;i < 15;i++) {
		oo.x = 62 * 20;
		oo.y = (i + 35) * 20;
		maparray[oo.x / 20][oo.y / 20] = 1;
		map2.push_back(oo);
	}
}

void create_map3()
{
	obj oo;
	for (int i = 0;i < 15;i++) {
		oo.x = 32 * 20;
		oo.y = (i + 5) * 20;
		maparray[oo.x / 20][oo.y / 20] = 1;
		map3.push_back(oo);
	}

	for (int i = 0;i < 15;i++) {
		oo.x = 62 * 20;
		oo.y = (i + 35) * 20;
		maparray[oo.x / 20][oo.y / 20] = 1;
		map3.push_back(oo);
	}

	for (int i = 0;i < 15;i++) {
		oo.x = 62 * 20;
		oo.y = (i + 5) * 20;
		maparray[oo.x / 20][oo.y / 20] = 1;
		map3.push_back(oo);
	}

	for (int i = 0;i < 15;i++) {
		oo.x = 32 * 20;
		oo.y = (i + 35) * 20;
		maparray[oo.x / 20][oo.y / 20] = 1;
		map3.push_back(oo);
	}
}

void create_snake() {
	obj oo;
	for (int i = 0; i < 10; i++) {
		oo.x = 460;
		oo.y = 500 - (i*20);
		snake1P.push_back(oo);
	}

	for (int i = 0; i < 10; i++) {
		oo.x = 1400;
		oo.y = 500 + (i*20);
		snake2P.push_back(oo);
	}
}

int create_food()
{

	obj oo;
	//if (food_spawn > food_spawn_time) {
	srand(time(NULL));
	if(foodspawn == true ){
		oo.x = (rand() % 68) + 10 + 1;
		oo.y = (rand() % 38) + 10 + 1;
		if (maparray[oo.x][oo.y] != 0) {
			return 0;
		}
		food.push_back(oo);
		foodspawn = false;
		food_clock.restart();
	}
	int t = food_clock.getElapsedTime().asSeconds();
	if (t > food_spawn_time) {
		foodspawn = true;
	}
}

int create_bomb()
{
	obj oo;
	if (bombspawn == true) {
		oo.x = (rand() % 68) + 10 + 1;
		oo.y = (rand() % 38) + 10 + 1;
		oo.time = time(NULL);
		if (maparray[oo.x][oo.y] != 0) {
			return 0;
		}
		bomb.push_back(oo);
		bombspawn = false;
		bomb_clock.restart();
	}
	int t = bomb_clock.getElapsedTime().asSeconds();
	if (t > bomb_spawn_time) {
		bombspawn = true;
	}
}

void explode_bomb(int i)
{
	sf::Texture texture;
	sf::RectangleShape bombpic(sf::Vector2f(140, 140));
	texture.loadFromFile("pic/explosion2-1.png", sf::IntRect(0, 128, 128, 128));
	bombpic.setTexture(&texture);
	bombpic.setPosition((bomb[i].x - 3) * 20, (bomb[i].y - 3) * 20);
	window.draw(bombpic);
}

void set_bombarea(int i, int a)
{
	/*maparray[bomb[i].x - 2][bomb[i].y - 2] = a;
	maparray[bomb[i].x - 1][bomb[i].y - 2] = a;
	maparray[bomb[i].x - 0][bomb[i].y - 2] = a;
	maparray[bomb[i].x + 1][bomb[i].y - 2] = a;
	maparray[bomb[i].x + 2][bomb[i].y - 2] = a;

	maparray[bomb[i].x - 2][bomb[i].y - 1] = a;
	maparray[bomb[i].x - 1][bomb[i].y - 1] = a;
	maparray[bomb[i].x - 0][bomb[i].y - 1] = a;
	maparray[bomb[i].x + 1][bomb[i].y - 1] = a;
	maparray[bomb[i].x + 2][bomb[i].y - 1] = a;

	maparray[bomb[i].x - 2][bomb[i].y - 0] = a;
	maparray[bomb[i].x - 1][bomb[i].y - 0] = a;
	maparray[bomb[i].x - 0][bomb[i].y - 0] = a;
	maparray[bomb[i].x + 1][bomb[i].y - 0] = a;
	maparray[bomb[i].x + 2][bomb[i].y - 0] = a;

	maparray[bomb[i].x - 2][bomb[i].y + 1] = a;
	maparray[bomb[i].x - 1][bomb[i].y + 1] = a;
	maparray[bomb[i].x - 0][bomb[i].y + 1] = a;
	maparray[bomb[i].x + 1][bomb[i].y + 1] = a;
	maparray[bomb[i].x + 2][bomb[i].y + 1] = a;

	maparray[bomb[i].x - 2][bomb[i].y + 2] = a;
	maparray[bomb[i].x - 1][bomb[i].y + 2] = a;
	maparray[bomb[i].x - 0][bomb[i].y + 2] = a;
	maparray[bomb[i].x + 1][bomb[i].y + 2] = a;
	maparray[bomb[i].x + 2][bomb[i].y + 2] = a;*/

	for (int j = 0;j < 5;j++) {
		for (int k = 0;k < 5;k++) {
			maparray[bomb[i].x + j - 2][bomb[i].y +k - 2] = a;
		}
	}
}

void create_obstacle(int i)
{
	obj oo;
	for (int j = 0;j < 5;j++) {
		for (int k = 0;k < 5;k++) {
			oo.x = bomb[i].x + j - 2;
			oo.y = bomb[i].y + k - 2;
			obstacle.push_back(oo);
		}
	}
}

void count_time_bomb()
{
	for (int i = 0;i < bomb.size();i++) {
		if (bomb[i].time - time(NULL) == -3 ) {
			if (bomb[i].explode == false) {
				// bomb sound effect
				sound_bomb.play();
				set_bombarea(i,1);
				bomb[i].explode = true;
			}
			explode_bomb(i);
			break;
		}
		else if (bomb[i].time - time(NULL) <= -4) {
			if (stage < 3) {
				set_bombarea(i, 0);
				del_bomb(bomb[i].x, bomb[i].y);
			}
			else {
				create_obstacle(i);
				del_bomb(bomb[i].x, bomb[i].y);
			}
		}
	}
}

void move_snake()
{
	// move snake player 1
	switch (snake1P_direction) {
	case UP: {
		
		/*if (maparray[snake1P[0].x / 20][snake1P[0].y / 20 - 1] == 1) {
			if (Numplayer == 1) {
				result = LOSE;
			}
			else {
				result = P2WIN;
			}
			break;
		}
		else if (maparray[snake1P[0].x / 20][snake1P[0].y / 20 - 1] == 2) {
			snake1P_eating = true;
			del_food(snake1P[0].x / 20, snake1P[0].y / 20 - 1);
		}
		else if (maparray[snake1P[0].x / 20][snake1P[0].y / 20 - 1] == 3) {
			result = DRAW;
			break;
		}*/
		//check_coliation
		if (check_coliation(snake1P[0].x, snake1P[0].y - 20 , 1) == 1) {
			break;
		}
		// move snake
		savebox.x = snake1P[0].x;
		savebox.y = snake1P[0].y;
		snake1P[0].y -= 20;
		maparray[snake1P[0].x/20][snake1P[0].y/20] = 3;
		for (int i = 1;i < snake1P.size(); i++) {
			temp.x = snake1P[i].x;
			temp.y = snake1P[i].y;
			snake1P[i].x = savebox.x;
			snake1P[i].y = savebox.y;
			maparray[snake1P[0].x/20][snake1P[0].y/20] = 1;
			savebox.x = temp.x;
			savebox.y = temp.y;
		}

		/*if (snake1P_eating == true) {
			obj oo;
			oo.x = savebox.x;
			oo.y = savebox.y;
			snake1P.push_back(oo);
			P1score++;
			snake1P_eating = false;
		}
		else {
			maparray[savebox.x / 20][savebox.y / 20] = 0;
		}*/
		//add 1 body box when eatting and deleat tail
		check_body(savebox.x, savebox.y, 1);
		break;
	}
	case DOWN: {
		//check_coliation
		if (check_coliation(snake1P[0].x, snake1P[0].y + 20, 1) == 1) {
			break;
		}
		// move snake

		savebox.x = snake1P[0].x;
		savebox.y = snake1P[0].y;
		snake1P[0].y += 20;
		maparray[snake1P[0].x / 20][snake1P[0].y / 20] = 3;
		for (int i = 1;i < snake1P.size(); i++) {
			temp.x = snake1P[i].x;
			temp.y = snake1P[i].y;
			snake1P[i].x = savebox.x;
			snake1P[i].y = savebox.y;
			maparray[snake1P[0].x / 20][snake1P[0].y / 20] = 1;
			savebox.x = temp.x;
			savebox.y = temp.y;
		}
		//add 1 body box when eatting and deleat tail
		check_body(savebox.x, savebox.y, 1);
		break;
	}
	case LEFT: {

		//check_coliation
		/*if (check_coliation(snake1P[0].x - 20, snake1P[0].y, 1) == 1) {
			break;
		}*/
		check_coliation(snake1P[0].x - 20, snake1P[0].y, 1);
		// move snake
		savebox.x = snake1P[0].x;
		savebox.y = snake1P[0].y;
		snake1P[0].x -= 20;
		maparray[snake1P[0].x / 20][snake1P[0].y / 20] = 3;
		for (int i = 1;i < snake1P.size(); i++) {
			temp.x = snake1P[i].x;
			temp.y = snake1P[i].y;
			snake1P[i].x = savebox.x;
			snake1P[i].y = savebox.y;
			maparray[snake1P[0].x / 20][snake1P[0].y / 20] = 1;
			savebox.x = temp.x;
			savebox.y = temp.y;
		}
		//add 1 body box when eatting and deleat tail
		check_body(savebox.x, savebox.y, 1);
		break;
	}
	case RIGHT: {
		//check_coliation
		if (check_coliation(snake1P[0].x + 20, snake1P[0].y, 1) == 1) {
			break;
		}

		// move snake
		savebox.x = snake1P[0].x;
		savebox.y = snake1P[0].y;
		snake1P[0].x += 20;
		maparray[snake1P[0].x / 20][snake1P[0].y / 20] = 3;
		for (int i = 1;i < snake1P.size(); i++) {
			temp.x = snake1P[i].x;
			temp.y = snake1P[i].y;
			snake1P[i].x = savebox.x;
			snake1P[i].y = savebox.y;
			maparray[snake1P[0].x / 20][snake1P[0].y / 20] = 1;
			savebox.x = temp.x;
			savebox.y = temp.y;
		}
		//add 1 body box when eatting and deleat tail
		check_body(savebox.x, savebox.y, 1);
		break;
	}
	default:;
	}

	//move snake player 2
	if (Numplayer == 2) {
		switch (snake2P_direction) {
		case UP: {
			//check_coliation
			if (check_coliation(snake2P[0].x, snake2P[0].y - 20, 2) == 1) {
				break;
			}

			// move snake
			savebox.x = snake2P[0].x;
			savebox.y = snake2P[0].y;
			snake2P[0].y -= 20;
			maparray[snake2P[0].x / 20][snake2P[0].y / 20] = 3;
			for (int i = 1;i < snake2P.size(); i++) {
				temp.x = snake2P[i].x;
				temp.y = snake2P[i].y;
				snake2P[i].x = savebox.x;
				snake2P[i].y = savebox.y;
				maparray[snake2P[0].x / 20][snake2P[0].y / 20] = 1;
				savebox.x = temp.x;
				savebox.y = temp.y;
			}
			//add 1 body box when eatting and deleat tail
			check_body(savebox.x, savebox.y, 2);
			break;
		}
		case DOWN: {
			//check_coliation
			if (check_coliation(snake2P[0].x, snake2P[0].y + 20, 2) == 1) {
				break;
			}

			// move snake
			savebox.x = snake2P[0].x;
			savebox.y = snake2P[0].y;
			snake2P[0].y += 20;
			maparray[snake2P[0].x / 20][snake2P[0].y / 20] = 3;
			for (int i = 1;i < snake2P.size(); i++) {
				temp.x = snake2P[i].x;
				temp.y = snake2P[i].y;
				snake2P[i].x = savebox.x;
				snake2P[i].y = savebox.y;
				maparray[snake2P[0].x / 20][snake2P[0].y / 20] = 1;
				savebox.x = temp.x;
				savebox.y = temp.y;
			}
			//add 1 body box when eatting and deleat tail
			check_body(savebox.x, savebox.y, 2);
			break;
		}
		case LEFT: {
			//check_coliation
			if (check_coliation(snake2P[0].x - 20, snake2P[0].y, 2) == 1) {
				break;
			}
			// move snake
			savebox.x = snake2P[0].x;
			savebox.y = snake2P[0].y;
			snake2P[0].x -= 20;
			maparray[snake2P[0].x / 20][snake2P[0].y / 20] = 3;
			for (int i = 1;i < snake2P.size(); i++) {
				temp.x = snake2P[i].x;
				temp.y = snake2P[i].y;
				snake2P[i].x = savebox.x;
				snake2P[i].y = savebox.y;
				maparray[snake2P[0].x / 20][snake2P[0].y / 20] = 1;
				savebox.x = temp.x;
				savebox.y = temp.y;
			}
			//add 1 body box when eatting and deleat tail
			check_body(savebox.x, savebox.y, 2);
			break;
		}
		case RIGHT: {
			//check_coliation
			if (check_coliation(snake2P[0].x + 20, snake2P[0].y, 2) == 1) {
				break;
			}
			// move snake
			savebox.x = snake2P[0].x;
			savebox.y = snake2P[0].y;
			snake2P[0].x += 20;
			maparray[snake2P[0].x / 20][snake2P[0].y / 20] = 3;
			for (int i = 1;i < snake2P.size(); i++) {
				temp.x = snake2P[i].x;
				temp.y = snake2P[i].y;
				snake2P[i].x = savebox.x;
				snake2P[i].y = savebox.y;
				maparray[snake2P[0].x / 20][snake2P[0].y / 20] = 1;
				savebox.x = temp.x;
				savebox.y = temp.y;
			}
			//add 1 body box when eatting and deleat tail
			check_body(savebox.x, savebox.y, 2);
			break;
		}
		default:;
		}
	}
}

void draw_obstacle()
{
	sf::Texture texture;
	sf::RectangleShape obstaclepic(sf::Vector2f(20, 20));
	texture.loadFromFile("pic/wall.png", sf::IntRect(0, 200, 100, 100));
	obstaclepic.setTexture(&texture);
	for (int i = 0;i < obstacle.size();i++) {
		obstaclepic.setPosition(obstacle[i].x * 20, obstacle[i].y * 20);
		maparray[obstacle[i].x][obstacle[i].y] = 1;
		window.draw(obstaclepic);
	}
}

void draw_food()
{
	sf::Texture texture;
	sf::RectangleShape foodpic(sf::Vector2f(20, 20));
	texture.loadFromFile("pic/candy.png", sf::IntRect(0,0, 400, 400));
	foodpic.setTexture(&texture);
	for (int i = 0;i < food.size();i++) {
		foodpic.setPosition(food[i].x*20, food[i].y*20);
		maparray[food[i].x][food[i].y] = 2;
		window.draw(foodpic);
	}
} 

void draw_bomb()
{
	sf::Texture texture;
	sf::RectangleShape bombpic(sf::Vector2f(20, 20));
	texture.loadFromFile("pic/bomb.png", sf::IntRect(0, 0, 178, 178));
	bombpic.setTexture(&texture);
	for (int i = 0;i < bomb.size();i++) {
		bombpic.setPosition(bomb[i].x * 20, bomb[i].y * 20);
		maparray[bomb[i].x][bomb[i].y] = 1;
		window.draw(bombpic);
	}
}

void draw_snake()
{
	sf::Texture texture;
	sf::RectangleShape snake(sf::Vector2f(20, 20));

	// draw head snake player 1
	snake.setPosition(snake1P[0].x, snake1P[0].y);
	maparray[snake1P[0].x / 20][snake1P[0].y / 20] = 3;
	texture.loadFromFile("pic/wall.png", sf::IntRect(200, 200, 100, 100));
	snake.setTexture(&texture);
	window.draw(snake);

	//set body snake player 1
	texture.loadFromFile("pic/wall.png", sf::IntRect(200, 200, 100, 100));
	snake.setTexture(&texture);
	for (int i = 1;i <snake1P.size();i++) {
		snake.setPosition(snake1P[i].x, snake1P[i].y);
		window.draw(snake);
		maparray[snake1P[i].x/20][snake1P[i].y/20] = 1;
	}

	if (Numplayer == 2) {
		//draw head snake player 2
		
		snake.setPosition(snake2P[0].x, snake2P[0].y);
		maparray[snake2P[0].x / 20][snake2P[0].y/20] = 3;
		texture.loadFromFile("pic/wall.png", sf::IntRect(200, 100, 100, 100));
		snake.setTexture(&texture);
		window.draw(snake);

		//set body snake player 2
		texture.loadFromFile("pic/wall.png", sf::IntRect(200, 100, 100, 100));
		snake.setTexture(&texture);
	
		for (int i = 1;i < snake2P.size();i++) {
			snake.setPosition(snake2P[i].x, snake2P[i].y);
			window.draw(snake);
			maparray[snake2P[i].x / 20][snake2P[i].y / 20] = 1;
		}
	}
}

void draw_wall()
{
	sf::Texture texture;
	texture.loadFromFile("pic/wall.png", sf::IntRect(200, 200, 100, 100));
	sf::RectangleShape wall_menu(sf::Vector2f(20, 20));
	wall_menu.setTexture(&texture);
	//wall_menu.setFillColor(sf::Color(0,0,0));
	//wall_menu.setOutlineThickness(5);
	//wall_menu.setOutlineColor(sf::Color(rand(), rand(), rand()));
	for (int i = 0;i < wall.size();i++) {
		wall_menu.setPosition(wall[i].x, wall[i].y);
		window.draw(wall_menu);
	}
}

void draw_map2()
{
	sf::Texture texture;
	texture.loadFromFile("pic/wall.png", sf::IntRect(200, 200, 100, 100));
	sf::RectangleShape wall_menu(sf::Vector2f(20, 20));
	wall_menu.setTexture(&texture);
	//wall_menu.setFillColor(sf::Color(0,0,0));
	//wall_menu.setOutlineThickness(5);
	//wall_menu.setOutlineColor(sf::Color(rand(), rand(), rand()));
	for (int i = 0;i < map2.size();i++) {
		wall_menu.setPosition(map2[i].x, map2[i].y);
		window.draw(wall_menu);
	}
}

void draw_map3()
{
	sf::Texture texture;
	texture.loadFromFile("pic/wall.png", sf::IntRect(200, 200, 100, 100));
	sf::RectangleShape wall_menu(sf::Vector2f(20, 20));
	wall_menu.setTexture(&texture);
	//wall_menu.setFillColor(sf::Color(0,0,0));
	//wall_menu.setOutlineThickness(5);
	//wall_menu.setOutlineColor(sf::Color(rand(), rand(), rand()));
	for (int i = 0;i < map3.size();i++) {
		wall_menu.setPosition(map3[i].x, map3[i].y);
		window.draw(wall_menu);
	}
}

void draw_pic_mainmenu()
{
	sf::Texture texture;
	sf::RectangleShape pic(sf::Vector2f(560, 560));
	pic.setPosition(Picred_menu_posX, 270);
	sf::RectangleShape picblue(sf::Vector2f(560, 560));
	picblue.setPosition(Picblue_menu_posX, 270);
	//Red snake
	switch (Picred_menu_direction) {
	case 0: {
		texture.loadFromFile("pic/snakered.png");
		if (Picred_menu_posX > 250) {
			pic.setPosition((Picred_menu_posX-=2), 270);
		}
		else {
			Picred_menu_direction = 1;
		}
		break;
	}
	case 1: {
		texture.loadFromFile("pic/snakered1.png");
		if (Picred_menu_posX < 350) {
			pic.setPosition((Picred_menu_posX += 2), 270);
		}
		else {
			Picred_menu_direction = 0;
		}
		break;
	}
	}
	pic.setTexture(&texture);
	window.draw(pic);

	//Blue snake
	switch (Picblue_menu_direction) {
	case 0: {
		texture.loadFromFile("pic/snakeblue.png");
		if (Picblue_menu_posX > 1000) {
			picblue.setPosition((Picblue_menu_posX -= 2), 270);
		}
		else {
			Picblue_menu_direction = 1;
		}
		break;
	}
	case 1: {
		texture.loadFromFile("pic/snakeblue1.png");
		if (Picblue_menu_posX < 1100) {
			picblue.setPosition((Picblue_menu_posX += 2), 270);
		}
		else {
			Picblue_menu_direction = 0;
		}
		break;
	}
	}
	/*if (Pic_menu_posX > 300 && Pic_menu_direction == 0) {
		texture.loadFromFile("pic/snake.png");
		pic.setPosition(Pic_menu_posX-=2, Pic_menu_poY);
	}
	else {

		printf("%d-1 ", Pic_menu_posX);
		pic.move(300, Pic_menu_poY);
		Pic_menu_direction = 1;
	}

	if (Pic_menu_posX < 480 && Pic_menu_direction == 1) {
		texture.loadFromFile("pic/snake.png");
		pic.setPosition(Pic_menu_posX+=2, Pic_menu_poY);
	}
	else {

		printf("%d-2 ",Pic_menu_posX);
		Pic_menu_posX = 480;
		pic.setPosition(480, 270);
		pic.move(480, Pic_menu_poY);
		Pic_menu_direction = 0;
	}*/
	picblue.setTexture(&texture);
	window.draw(picblue);

}

void draw_pic_mapselectmenu()
{
	sf::Texture texture;
	sf::RectangleShape picmap1(sf::Vector2f(380, 280));
	texture.loadFromFile("pic/map1.png", sf::IntRect(0, 0, 1541, 925));
	picmap1.setTexture(&texture);
	picmap1.setPosition(400, 300);
	window.draw(picmap1);

	texture.loadFromFile("pic/map2.png", sf::IntRect(0, 0, 1541, 925));
	picmap1.setTexture(&texture);
	picmap1.setPosition(1200, 300);
	window.draw(picmap1);

	texture.loadFromFile("pic/map3.png", sf::IntRect(0, 0, 1541, 925));
	picmap1.setTexture(&texture);
	picmap1.setPosition(400, 650);
	window.draw(picmap1);

	sf::RectangleShape picmapwtf(sf::Vector2f(380, 280));
	texture.loadFromFile("pic/face3.png", sf::IntRect(0, 0, 1541, 925));
	picmapwtf.setTexture(&texture);
	picmapwtf.setPosition(1200, 650);
	window.draw(picmapwtf);
}

void draw_pic_resultsingle()
{
	sf::Texture texture;
	sf::RectangleShape picface(sf::Vector2f(600, 600));
	texture.loadFromFile("pic/face2.png");
	picface.rotate(10);
	//picface.getOrigin(sf::Vector2f(texture.getSize().x / 2, texture.getSize().y / 2));
	picface.setTexture(&texture);
	picface.setPosition(750, 100);
	window.draw(picface);

	texture.loadFromFile("pic/noob.png");
	picface.rotate(10);
	//picface.getOrigin(sf::Vector2f(texture.getSize().x / 2, texture.getSize().y / 2));
	picface.setTexture(&texture);
	picface.setPosition(800, 100);
	window.draw(picface);
}

void draw_pic_resultmuti()
{
	sf::Texture texture;
	sf::RectangleShape picface(sf::Vector2f(500, 500));
	sf::RectangleShape picface2(sf::Vector2f(500, 500));
	if (result == P1WIN) {
		texture.loadFromFile("pic/face1.png");
		picface.rotate(10);
		picface.setPosition(400, 150);
	}
	else {
		texture.loadFromFile("pic/face5.png");
		picface2.setScale(-1, 1);
		picface.setPosition(300, 200);
	}

	//picface.getOrigin(sf::Vector2f(texture.getSize().x / 2, texture.getSize().y / 2));
	picface.setTexture(&texture);
	
	window.draw(picface);

	if (result != P2WIN) {
		texture.loadFromFile("pic/face5.png");
		picface2.setScale(-1, 1);
		picface2.setPosition(1650, 200);
	}
	else {
		texture.loadFromFile("pic/face1.png");
		picface.rotate(10);
		picface2.setPosition(1650, 200);
	}

	picface2.setTexture(&texture);
	
	window.draw(picface2);
}

void draw_text_mainmenu()
{
	sf::RectangleShape box;
	box.setFillColor(sf::Color::White);
	sf::Font font;
	font.loadFromFile("font/8bit.ttf");
	sf::Text text;
	text.setFont(font);

	//draw_select box
	if (Now_select == 1) {
		box.setSize(sf::Vector2f(230, 50));
		box.setPosition(840, 390);
		window.draw(box);
	}
	if (Now_select == 2) {
		box.setSize(sf::Vector2f(240, 50));
		box.setPosition(840, 590);
		window.draw(box);
	}
	if (Now_select == 3) {
		box.setSize(sf::Vector2f(120, 50));
		box.setPosition(890, 790);
		window.draw(box);
	}

	text.setString("HOT HEAD SNAKE BATTLE");
	text.setCharacterSize(70);
	text.setColor(sf::Color::Red);
	text.setPosition(280, 150);
	window.draw(text);

	text.setString("1 player");
	text.setCharacterSize(30);
	text.setColor(sf::Color::Red);
	text.setPosition(850, 400);
	window.draw(text);

	text.setString("2 player");
	text.setCharacterSize(30);
	text.setColor(sf::Color::Red);
	text.setPosition(850, 600);
	window.draw(text);

	text.setString("QUIT");
	text.setCharacterSize(30);
	text.setColor(sf::Color::Red);
	text.setPosition(900, 800);
	window.draw(text);

	text.setString("Create by SUPAVITCH SANGSUWAN : Student ID 59011341");
	text.setCharacterSize(15);
	text.setColor(sf::Color::White);
	text.setPosition(950, 1000);
	window.draw(text);


}

void draw_text_mapselectmenu() 
{
	sf::RectangleShape box;
	box.setFillColor(sf::Color::White);
	sf::Font font;
	font.loadFromFile("font/8bit.ttf");
	sf::Text text;
	text.setFont(font);

	//draw_select box
	if (Now_select == 1) {
		box.setSize(sf::Vector2f(125, 30));
		box.setPosition(395, 260);
		window.draw(box);
	}
	if (Now_select == 2) {
		box.setSize(sf::Vector2f(135, 30));
		box.setPosition(1195, 260);
		window.draw(box);
	}
	if (Now_select == 3) {
		box.setSize(sf::Vector2f(135, 30));
		box.setPosition(395, 600);
		window.draw(box);
	}
	if (Now_select == 4) {
		box.setSize(sf::Vector2f(200, 30));
		box.setPosition(1195, 600);
		window.draw(box);
	}
	
	text.setString("MAP SELECTION MENU");
	text.setCharacterSize(70);
	text.setColor(sf::Color::Red);
	text.setPosition(360, 150);
	window.draw(text);

	text.setString("MAP 1");
	text.setCharacterSize(25);
	text.setColor(sf::Color::Red);
	text.setPosition(400, 260);
	window.draw(text);

	text.setString("MAP 2");
	text.setCharacterSize(25);
	text.setColor(sf::Color::Red);
	text.setPosition(1200, 260);
	window.draw(text);

	text.setString("MAP 3");
	text.setCharacterSize(25);
	text.setColor(sf::Color::Red);
	text.setPosition(400, 600);
	window.draw(text);

	text.setString("WTF MAP");
	text.setCharacterSize(25);
	text.setColor(sf::Color::Red);
	text.setPosition(1200, 600);
	window.draw(text);

}

void draw_text_map()
{
	sf::Font font;
	font.loadFromFile("font/8bit.ttf");
	sf::Text text;
	text.setFont(font);


	text.setString("STAGE ");
	text.setCharacterSize(40);
	text.setColor(sf::Color::Red);
	text.setPosition(800, 20);
	window.draw(text);

	text.setString(std::to_string(stage));
	text.setCharacterSize(40);
	text.setColor(sf::Color::Red);
	text.setPosition(1050, 20);
	window.draw(text);

	text.setString("Player 1");
	text.setCharacterSize(20);
	text.setColor(sf::Color::Red);
	text.setPosition(45, 100);
	window.draw(text);

	text.setString("Score");
	text.setCharacterSize(20);
	text.setColor(sf::Color::Red);
	text.setPosition(45, 140);
	window.draw(text);

	text.setString(std::to_string(P1score));
	text.setCharacterSize(20);
	text.setColor(sf::Color::Red);
	text.setPosition(45, 180);
	window.draw(text);

	text.setString(" ( P ) pause  ( M ) menu");
	text.setCharacterSize(15);
	text.setColor(sf::Color::White);
	text.setPosition(1200, 1000);
	window.draw(text);


	if (Numplayer == 2) {
		text.setString("Player 2");
		text.setCharacterSize(20);
		text.setColor(sf::Color::Red);
		text.setPosition(1750, 100);
		window.draw(text);

		text.setString("Score");
		text.setCharacterSize(20);
		text.setColor(sf::Color::Red);
		text.setPosition(1750, 140);
		window.draw(text);

		text.setString(std::to_string(P2score));
		text.setCharacterSize(20);
		text.setColor(sf::Color::Red);
		text.setPosition(1750, 180);
		window.draw(text);
	}

}

void draw_text_resultsingle()
{
	sf::Font font;
	font.loadFromFile("font/8bit.ttf");
	sf::Text text;
	text.setFont(font);

	text.setString("YOUR SCORE ");
	text.setCharacterSize(60);
	text.setColor(sf::Color::Red);
	text.setPosition(700, 700);
	window.draw(text);

	text.setString(std::to_string(P1score));
	text.setCharacterSize(60);
	text.setColor(sf::Color::Yellow);
	text.setPosition(950, 800);
	window.draw(text);

	text.setString("press M to main menu");
	text.setCharacterSize(30);
	text.setColor(sf::Color::Red);
	text.setPosition(700, 900);
	window.draw(text);


}

void draw_text_resultmuti()
{
	sf::Font font;
	font.loadFromFile("font/8bit.ttf");
	sf::Text text;
	text.setFont(font);
	if (result == P1WIN) {
		text.setString("Player 1 WIN");
		text.setCharacterSize(60);
		text.setColor(sf::Color::Yellow);
		text.setPosition(650, 700);
		window.draw(text);
	}
	if (result == P2WIN){
		text.setString("Player 2 WIN");
		text.setCharacterSize(60);
		text.setColor(sf::Color::Yellow);
		text.setPosition(650, 700);
		window.draw(text);
	}
	if (result == DRAW) {
		text.setString("DRAW");
		text.setCharacterSize(60);
		text.setColor(sf::Color::Yellow);
		text.setPosition(850, 700);
		window.draw(text);
	}
	text.setString("Player 1 score");
	text.setCharacterSize(30);
	text.setColor(sf::Color::Red);
	text.setPosition(320, 800);
	window.draw(text);

	text.setString(std::to_string(P1score));
	text.setCharacterSize(30);
	text.setColor(sf::Color::Yellow);
	text.setPosition(500, 850);
	window.draw(text);

	text.setString("Player 2 score");
	text.setCharacterSize(30);
	text.setColor(sf::Color::Red);
	text.setPosition(1200, 800);
	window.draw(text);

	text.setString(std::to_string(P2score));
	text.setCharacterSize(30);
	text.setColor(sf::Color::Yellow);
	text.setPosition(1350, 850);
	window.draw(text);

	text.setString("press M to main menu");
	text.setCharacterSize(30);
	text.setColor(sf::Color::Red);
	text.setPosition(700, 900);
	window.draw(text);
}

void renderFN()
{
	while (window.isOpen()) {
		window.clear(sf::Color::Black);
		// render main menu
		if (Now_screen == MAINMENU) {
			draw_wall();
			draw_pic_mainmenu();
			draw_text_mainmenu();
		}

		// render map select menu
		if (Now_screen == MAPSELECTMENU) {
			draw_wall();
			draw_pic_mapselectmenu();
			draw_text_mapselectmenu();

		}

		// render map 1 2 3
		if (Now_screen == MAP1 || Now_screen == MAP2 || Now_screen == MAP3) {

			if ((time(NULL) - start_time) > 20) {
				gamespeed = 25;
				stage = 2;
				bomb_spawn_time = 2;
				if ((time(NULL) - start_time) > 50) {
					gamespeed = 1;
					stage = 3;
					food_spawn_time = 0;
				}
			}
			//printf("%d\n", time(NULL) - start_time);
			if (pause % 2 == 0) {
				if (Now_screen == MAP2) {
					draw_map2();
				}
				if (Now_screen == MAP3) {
					draw_map3();
				}
				//nloop++;
				draw_wall();
				create_food();
				draw_food();
				draw_obstacle();
				
				
				if (stage > 1) {
					create_bomb();
					draw_bomb();
					count_time_bomb();
				}
				move_snake();
				snake1P_canturn = true;
				snake2P_canturn = true;
				draw_snake();
				check_result();
				draw_text_map();
				Sleep(gamespeed);
			}
			else {
				if (Now_screen == MAP2) {
					draw_map2();
				}
				if (Now_screen == MAP3) {
					draw_map3();
				}
				draw_bomb();
				draw_wall();
				draw_obstacle();
				draw_food();
				draw_snake();
				draw_text_map();
				Sleep(gamespeed);
			}
		}

		if (Now_screen == MAP_WTF) {
			draw_wall();
			create_food();
			draw_food();
			draw_obstacle();
			
			create_bomb();
			draw_bomb();
			count_time_bomb();
			move_snake();
			snake1P_canturn = true;
			snake2P_canturn = true;
			draw_snake();
			check_result();
			draw_text_map();
			Sleep(gamespeed);
		}

		if (Now_screen == SCORERESULTSINGLE) {
		//	printf("SINGLE-%d \n",result);
			draw_wall();
			draw_text_resultsingle();
			draw_pic_resultsingle();
		}

		if (Now_screen == SCORERESULTMUTI) {
		//	printf("MUTI-%d \n",result);
			draw_wall();
			draw_text_resultmuti();
			draw_pic_resultmuti();
		}

		window.display();

	}
}

int main()
{
 
	//float micro = time.asMicroseconds();
	window.setFramerateLimit(60);
	window.setActive(false);

	create_wall();
	create_snake();

	sf::Music music;
	music.openFromFile("sound/bgm/bgm.ogg");
	music.play();
	music.setLoop(true);


	sf::SoundBuffer buffer_bomb;
	buffer_bomb.loadFromFile("sound/effect/bomb.ogg");
	sf::SoundBuffer buffer_beep;
	buffer_beep.loadFromFile("sound/effect/beep.ogg");
	sound_bomb.setBuffer(buffer_bomb);
	sound_beep.setBuffer(buffer_beep);
	sf::Thread renderthread(&renderFN);
	renderthread.launch();


	while (window.isOpen()) {
		while (window.pollEvent(event)) {
			// main menu input 
			if (Now_screen == MAINMENU) {
				
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
					printf("w ");
					if (Now_select == 1) {
						Now_select = 3;
					}
					else if (Now_select == 2) {
						Now_select = 1;
					}
					else if (Now_select == 3) {
						Now_select = 2;
					}
					Sleep(50);
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
					printf("a ");

				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
					printf("s ");
					if (Now_select == 1) {
						Now_select = 2;
					}
					else if (Now_select == 2) {
						Now_select = 3;
					}
					else if (Now_select == 3) {
						Now_select = 1;
					}
					Sleep(50);
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
					printf("d ");

				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) {
					if (Now_select == 1) {
						Now_select = 1;
						oo_obstacle.x = 85;
						oo_obstacle.y = 48;
						gamespeed = 50;
						stage = 1;
						nloop = 1;
						P1score = 0;
						P2score = 0;
						Now_screen = MAPSELECTMENU;
						Numplayer = 1;
						food_spawn_time = 3;
						result = 0;
						obstacle.clear();
						clear_map();
						window.clear();
						snake1P_canturn = true;
						snake1P_direction = DOWN;
						snake1P_eating = false;
						create_wall();
						create_snake();
						Now_select = 1;
						printf("MAP1");
						Sleep(100);
					}
					else if (Now_select == 2) {
						gamespeed = 50;
						Now_select = 1;
						nloop = 1;
						stage = 1;
						P1score = 0;
						P2score = 0;
						Now_screen = MAPSELECTMENU;
						Numplayer = 2;
						food_spawn_time = 1;
						result = 0;
						obstacle.clear();
						clear_map();
						window.clear();
						snake1P_canturn = true;
						snake1P_direction = DOWN;
						snake1P_eating = false;
						snake2P_canturn = true;
						snake2P_direction = UP;
						snake2P_eating = false;
						create_wall();
						create_snake();
						Now_select = 1;
						printf("MAP1 2 player");
						Sleep(100);
					}
					else if (Now_select == 3) {
						window.close();
						return 0;
					}
					Sleep(50);
				}
			}

			if (Now_screen == MAPSELECTMENU) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
					printf("w ");
					if (Now_select == 3) {
						Now_select = 1;
					}
					else if (Now_select == 4) {
						Now_select = 2;
					}
					Sleep(50);
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
					printf("a ");
					if (Now_select == 2) {
						Now_select = 1;
					}
					else if (Now_select == 4) {
						Now_select = 3;
					}
					Sleep(50);
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
					printf("s ");
					if (Now_select == 1) {
						Now_select = 3;
					}
					else if (Now_select == 2) {
						Now_select = 4;
					}
					Sleep(50);
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
					printf("d ");
					if (Now_select == 1) {
						Now_select = 2;
					}
					else if (Now_select == 3) {
						Now_select = 4;
					}
					Sleep(50);
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) {
					start_clock.restart();
					start_time = time(NULL);
					if (Now_select == 1) {
						Now_screen = MAP1;
					}
					else if (Now_select == 2) {
						Now_screen = MAP2;
						create_map2();
					}
					else if (Now_select == 3) {
						Now_screen = MAP3;
						create_map3();
					}
					else if (Now_select == 4) {
						Now_screen = MAP_WTF;
						food_spawn_time = 0;
						bomb_spawn_time = 0;
						stage = 999;
						gamespeed = 0;

					}
					Sleep(500);
				}
			}

			// MAP1 input
			if (Now_screen == MAP1 || Now_screen == MAP2 || Now_screen == MAP3 || Now_screen == MAP_WTF) {
				//printf("MAP1");
				//snake player 1
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && snake1P_canturn) { // go Up
					if (snake1P_direction != DOWN && snake1P_canturn == true) {
						snake1P_direction = UP;
						snake1P_canturn = false;
					}
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && snake1P_canturn == true) { // go LEFT
					if (snake1P_direction != RIGHT) {
						snake1P_direction = LEFT;
						snake1P_canturn = false;
					}
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && snake1P_canturn == true) { // go DOWN
					if (snake1P_direction != UP) {
						snake1P_direction = DOWN;
						snake1P_canturn = false;
					}
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && snake1P_canturn == true) { // go RIGHT
					if (snake1P_direction != LEFT) {
						snake1P_direction = RIGHT;
						snake1P_canturn = false;
					}
				}

				//snake player 2
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && snake2P_canturn == true ) { // go Up
					if (snake2P_direction != DOWN ) {
						snake2P_direction = UP;
						snake2P_canturn = false;
					}
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && snake2P_canturn == true) { // go LEFT
					if (snake2P_direction != RIGHT ) {
						snake2P_direction = LEFT;
						snake2P_canturn = false;
					}
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && snake2P_canturn == true) { // go DOWN
					if (snake2P_direction != UP ) {
						snake2P_direction = DOWN;
						snake2P_canturn = false;
					}
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && snake2P_canturn == true) { // go RIGHT
					if (snake2P_direction != LEFT ) {
						snake2P_direction = RIGHT;
						snake2P_canturn = false;
					}
				}
			}




			// ESC for exit
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
				window.close();
				return 0;
			}
			// print map for debug
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace)) {
				check_maparray();
			}
			// pause in game mode
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
				pause++;
				Sleep(100);
			}
			// press M for back to main menu
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::M)) {
				Now_screen = MAINMENU;
				Now_select = 1;
			}
			Sleep(50);
		}
	}
    return 0;
}

