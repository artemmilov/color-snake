#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

using namespace std;

inline float min(float a, float b)
{
	if (a <= b)
		return a;
	else
		return b;
}

class field_class
{
private:
	unsigned int size;
	vector <string> scheme;

	pair <unsigned int, unsigned int> apple_coord;
public:
	bool init(unsigned int SZ, vector <string>* SCH)
	{
		if (SZ == 0)
			return false;
		size = SZ;

		int num_apple = 0;
		for (int i(0); i < SCH->size(); i++)
		{
			if ((*SCH)[i].length() != size)
				return false;
			for (int j(0); j < size; j++)
			{
				if ((*SCH)[i][j] != '0' && (*SCH)[i][j] != 'R' && (*SCH)[i][j] != 'G' && (*SCH)[i][j] != 'B' && (*SCH)[i][j] != 'W')
					return false;
				if ((*SCH)[i][j] == 'R' || (*SCH)[i][j] == 'G' || (*SCH)[i][j] == 'B')
				{
					apple_coord.first = i;
					apple_coord.second = j;
					num_apple++;
				}
			}
		}
		if (num_apple != 1)
			return false;
		scheme = *SCH;
		return true;
	}
	bool take_apple(vector <pair <unsigned int, unsigned int>> snake_coords)
	{
		scheme[apple_coord.first][apple_coord.second] = '0';
		apple_coord.first = rand() % size;
		apple_coord.second = rand() % size;
		for (int i(0); i < snake_coords.size(); i++)
			if (apple_coord == snake_coords[i] || scheme[apple_coord.first][apple_coord.second] != '0')
			{
				apple_coord.first = rand() % size;
				apple_coord.second = rand() % size;
				i = 0;
			}
			int r = rand() % 3;
			switch (r)
			{
			case 0:
				scheme[apple_coord.first][apple_coord.second] = 'R';
				break;
			case 1:
				scheme[apple_coord.first][apple_coord.second] = 'G';
				break;
			case 2:
				scheme[apple_coord.first][apple_coord.second] = 'B';
				break;
			}
		return true;
	}

	bool update(float time)
	{
		const float step_time = 0.5;
		static float cur_time = 0;
		cur_time += min(time, 0.005);
		if (cur_time >= step_time)
		{
			cur_time = 0;

			if (scheme[apple_coord.first][apple_coord.second] == 'R')
				scheme[apple_coord.first][apple_coord.second] = 'G';
			else if (scheme[apple_coord.first][apple_coord.second] == 'G')
				scheme[apple_coord.first][apple_coord.second] = 'B';
			else if (scheme[apple_coord.first][apple_coord.second] == 'B')
				scheme[apple_coord.first][apple_coord.second] = 'R';
		}

		return true;
	}

	unsigned int get_size()
	{
		return size;
	}
	char what_in_cell(pair <unsigned int, unsigned int> CELL_crd)
	{
		if (CELL_crd.first >= size || CELL_crd.second >= size)
			return 'E';
		return scheme[CELL_crd.first][CELL_crd.second];
	}
};

struct snake_part
{
	pair <unsigned int, unsigned int> coord;
	char color;
};
class snake_class
{
private:
	vector <snake_part> parts;
	char dir;
	field_class* field;
	bool grow;
	char color_grow;
	bool alive;

	int success;

	bool controlable;

	bool correct_form()
	{
		unsigned int same_color_counter = 1;
		const unsigned int cut_value = 3;
		for (int i(1); i < parts.size(); i++)
		{
			if ((parts[i - 1].coord.first - parts[i].coord.first) * (parts[i - 1].coord.first - parts[i].coord.first) + (parts[i - 1].coord.second - parts[i].coord.second) * (parts[i - 1].coord.second - parts[i].coord.second) != 1)
				return false;
			for (int j(1); j < i; j++)
				if (parts[j].coord == parts[i].coord)
					return false;
			if (parts[i - 1].color == parts[i].color)
				same_color_counter++;
			else
				same_color_counter = 1;
			if (same_color_counter == cut_value)
			{
				same_color_counter = 0;
				for (int j(0); j < cut_value; j++)
					parts.pop_back();
			}
		}
		return true;
	}
	bool interact_with_field()
	{
		snake_part head_part = parts[0];

		if (field->what_in_cell(head_part.coord) == 'R' || field->what_in_cell(head_part.coord) == 'G' || field->what_in_cell(head_part.coord) == 'B')
		{
			grow = true;
			color_grow = field->what_in_cell(head_part.coord);
			vector <pair <unsigned int, unsigned int>> arg_coords;
			arg_coords.clear();
			for (int i(0); i < parts.size(); i++)
				arg_coords.push_back(parts[i].coord);
			field->take_apple(arg_coords);
		}
		if (field->what_in_cell(head_part.coord) == 'W')
			alive = false;
		if (field->what_in_cell(head_part.coord) == 'E')
			return false;
		return true;
	}
public:
	bool init(field_class* FLD, pair <unsigned int, unsigned int> start_coord, char start_dir)
	{
		field = FLD;

		if (start_dir != 'U' && start_dir != 'D' && start_dir != 'L' && start_dir != 'R')
			return false;
		dir = start_dir;

		if (start_coord.first < 0 || start_coord.second < 0 || start_coord.first > field->get_size() - 1 || start_coord.second > field->get_size() - 1)
			return false;
		parts.clear();
		snake_part start_part;
		start_part.coord = start_coord;
		start_part.color = 'T';
		parts.push_back(start_part);

		grow = false;
		alive = true;
		controlable = true;

		success = -1;

		return true;
	}

	void left()
	{
		if (dir != 'R' && dir != 'L' && controlable)
		{
			dir = 'L';
			controlable = false;
		}
	}
	void right()
	{
		if (dir != 'L' && dir != 'R' && controlable)
		{
			dir = 'R';
			controlable = false;
		}
	}
	void up()
	{
		if (dir != 'D' && dir != 'U' && controlable)
		{
			dir = 'U';
			controlable = false;
		}
	}
	void down()
	{
		if (dir != 'U' && dir != 'D' && controlable)
		{
			dir = 'D';
			controlable = false;
		}
	}

	bool update(float time)
	{
		const float step_time = 0.025;
		static float cur_time = 0;
		cur_time += min(time, 0.005);
		if (cur_time >= step_time)
		{
			cur_time = 0;
			controlable = true;
			if (alive && success != 1)
			{
				snake_part head_part = parts[0];
				snake_part new_head_part;
				if (dir == 'U')
				{
					new_head_part.coord.first = head_part.coord.first ;
					new_head_part.coord.second = (field->get_size() + head_part.coord.second - 1) % field->get_size();
				}
				if (dir == 'D')
				{
					new_head_part.coord.first = head_part.coord.first;
					new_head_part.coord.second = (head_part.coord.second + 1) % field->get_size();
				}
				if (dir == 'L')
				{
					new_head_part.coord.first = (field->get_size() + head_part.coord.first - 1) % field->get_size();
					new_head_part.coord.second = head_part.coord.second;
				}
				if (dir == 'R')
				{
					new_head_part.coord.first = (head_part.coord.first + 1) % field->get_size();
					new_head_part.coord.second = head_part.coord.second;
				}
				parts.insert(parts.begin(), new_head_part);
				new_head_part.color = 'T';
				for (int i(1); i < parts.size() - 1; i++)
					parts[i].color = parts[i + 1].color;
				if (!grow)
					parts.pop_back();
				else
				{
					grow = false;
					if (success == -1)
						success = 0;
					parts[parts.size() - 1].color = color_grow;
				}
			}
		}

		for (int i(1); i < parts.size(); i++)
			if (parts[0].coord == parts[i].coord)
				alive = false;

		if (!interact_with_field())
			return false;
		if (!correct_form())
			return false;

		if (parts.size() == 1 && success == 0)
			success = 1;

		return true;
	}

	vector <snake_part> get_parts()
	{
		return parts;
	}
	char get_dir()
	{
		return dir;
	}
	bool is_alive()
	{
		return alive;
	}
	bool is_win()
	{
		return success == 1;
	}
};

class drawer_class
{
	sf::Image snake_image;
	sf::Texture snake_texture;
	sf::Sprite snake_sprite;

	sf::Image field_image;
	sf::Texture field_texture;
	sf::Sprite field_sprite;

	int tile_size;

	sf::Text result_text;
	sf::Font font;

	sf::RenderWindow* window;
public:
	bool init(sf::RenderWindow* WNDW)
	{
		if (!snake_image.loadFromFile("images/snake_tiles.png"))
			return false;
		if (!snake_texture.loadFromImage(snake_image))
			return false;
		snake_sprite.setTexture(snake_texture);
		if (!field_image.loadFromFile("images/field_tiles.png"))
			return false;
		if (!field_texture.loadFromImage(field_image))
			return false;
		field_sprite.setTexture(field_texture);
		tile_size = 10;

		if (!font.loadFromFile("fonts/font.ttf"))
			return false;
		result_text.setFont(font);
		result_text.setCharacterSize(120);
		result_text.setStyle(sf::Text::Bold);

		window = WNDW;
		
		return true;
	}
	bool draw_field(field_class* field)
	{
		int sz = field->get_size();

		for (unsigned int i(0); i < sz; i++)
			for (unsigned int j(0); j < sz; j++)
			{
				switch (field->what_in_cell(pair <unsigned int, unsigned int>(i, j)))
				{
				case '0':
					field_sprite.setTextureRect(sf::IntRect(0, 0, tile_size, tile_size));
					break;
				case 'W':
					field_sprite.setTextureRect(sf::IntRect(10, 0, tile_size, tile_size));
					break;
				case 'R':
					field_sprite.setTextureRect(sf::IntRect(20, 0, tile_size, tile_size));
					break;
				case 'G':
					field_sprite.setTextureRect(sf::IntRect(30, 0, tile_size, tile_size));
					break;
				case 'B':
					field_sprite.setTextureRect(sf::IntRect(40, 0, tile_size, tile_size));
					break;
				default:
					return false;
				}
				field_sprite.setPosition(i*tile_size, j*tile_size);
				window->draw(field_sprite);
			}
		return true;
	}
	bool draw_snake(snake_class* snake)
	{
		for (auto part : snake->get_parts())
		{
			if (snake->is_alive())
			{
				if (part.color == 'R')
					snake_sprite.setTextureRect(sf::IntRect(20, 0, tile_size, tile_size));
				if (part.color == 'G')
					snake_sprite.setTextureRect(sf::IntRect(30, 0, tile_size, tile_size));
				if (part.color == 'B')
					snake_sprite.setTextureRect(sf::IntRect(40, 0, tile_size, tile_size));
			}
			else
				snake_sprite.setTextureRect(sf::IntRect(10, 0, tile_size, tile_size));
			snake_sprite.setPosition(part.coord.first*tile_size, part.coord.second*tile_size);
			window->draw(snake_sprite);
		}
		if (snake->is_alive())
			snake_sprite.setTextureRect(sf::IntRect(0, 0, tile_size, tile_size));
		else
			snake_sprite.setTextureRect(sf::IntRect(10, 0, tile_size, tile_size));
		snake_sprite.setPosition(snake->get_parts()[0].coord.first*tile_size, snake->get_parts()[0].coord.second*tile_size);
		window->draw(snake_sprite);
		return true;
	}
	bool draw_info(snake_class* snake)
	{		
		if (!snake->is_alive())
		{
			result_text.setFillColor(sf::Color::Red);
			result_text.setString("You lose!");
			result_text.setPosition((window->getSize().x - result_text.getGlobalBounds().width) / 2.0, (window->getSize().y - result_text.getGlobalBounds().height) / 2.0);
			window->draw(result_text);
		}
		if (snake->is_win())
		{
			result_text.setFillColor(sf::Color::Green);
			result_text.setString("You win!");
			result_text.setPosition((window->getSize().x - result_text.getGlobalBounds().width) / 2.0, (window->getSize().y - result_text.getGlobalBounds().height) / 2.0);
			window->draw(result_text);
		}
		return true;
	}
};

int main()
{
	sf::RenderWindow window(sf::VideoMode(750, 750), "Color snake");

	vector <string> scheme = { 
							"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000000R0000000000000000000000000000000W",/////
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W000000000000000WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W00000000000000000000000000000000000000W0000000000000000000000000000000000W",
							"W0000000000000000000000000000000000000000000000000000000000000000000000000W",
							"W0000000000000000000000000000000000000000000000000000000000000W00000000000W",
							"W0000000000000000000000000000000000000000000000000000000000000W00000000000W",
							"W0000000000000000000000000000000000000000000000000000000000000W00000000000W",
							"W000000000000000000000000000000000000000000000000000000000WWWWWWWW00000000W",
							"W0000000000000000000000000000000000000000000000000000000000000W00000000000W",
							"W0000000000000000000000000000000000000000000000000000000000000W00000000000W",
							"W0000000000000000000000000000000000000000000000000000000000000W00000000000W",
							"W0000000000000000000000000000000000000000000000000000000000000000000000000W",
							"W0000000000000000000000000000000000000000000000000000000000000000000000000W",
							"W0000000W00000000000000000000000000000000000000000000000000000000000000000W",
							"W0000000W00000000000000000000000000000000000000000000000000000000000000000W",
							"W0000000W00000000000000000000000000000000000000000000000000000000000000000W",
							"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW0000000000000000000000000000W",
							"W0000000000000000000000000000000000000000000000000000000000000000000000000W",
							"W000000000000000000000000000000000000000000000000000000000000W000000000000W",
							"W000000000000000000000000000000000000000000000000000000000000W000000000000W",
							"W000000000000000000000000000000000000000000000000000000000000W000000000000W",
							"W000000000000000000000000000000000000000000000000000000000000W000000000000W",
							"W000000000000000000000000000000000000000000000000000000000000W000000000000W",
							"W000000000000000000000000000000000000000000000000000000000000W000000000000W",
							"W0000000000000000000000000000000WWWWWWWWWWWWWWWWWWWWWWWWWWWWWW000000000000W",
							"W00000000000000000000000000000000000000000000000000000W0000000000000000000W",
							"W00000000000000000000000000000000000000000000000000000W0000000000000000000W",
							"W00000000000000000000000000000000000000000000000000000W0000000000000000000W",
							"000000000000000000000000000000000000000000000000000000W00000000000000000000",
							"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
							};
	field_class field;
	field.init(75, &scheme);

	snake_class snake;
	snake.init(&field, pair <unsigned int, unsigned int> (1,1), 'R');

	drawer_class drawer;
	drawer.init(&window);


	sf::Clock clock;
	float time = 0;
	while (window.isOpen())
	{
		time = clock.getElapsedTime().asSeconds();
		clock.restart();
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			snake.left();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			snake.right();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			snake.up();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			snake.down();

		field.update(time);
		snake.update(time);

		window.clear();
		drawer.draw_field(&field);
		drawer.draw_snake(&snake);
		drawer.draw_info(&snake);
		window.display();
	}

	return 0;
}