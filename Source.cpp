/* ---------------------------------------------------------------------------
* Copyright &copy; 2016 Hunter Straub <Hunterstraub@csu.fullerton.edu>
* Released under the [MIT License] (http://opensource.org/licenses/MIT)
* -------------------------------------------------------------------------*/

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <cmath>
using std::cout;
using std::endl;
using std::vector;

//Constants
const int TERM_SIZE_X = 80;
const int TERM_SIZE_Y = 25;
const int SIZE = TERM_SIZE_X * TERM_SIZE_Y;

class Point
{
	int x, y;
	char c;
public:
	Point(int X = 0, int Y = 0, char C = ' ')
	{
		x = X;
		y = Y;
		c = C;
	}
	int getX() { return x; };
	int getY() { return y; };
	char getChar() { return c; };
	void setChar(char C) { c = C; };
};

class Shape
{
public:
	Point * data_;

	Shape()
	{
		data_ = new Point[SIZE];
	}

	const Point operator [] (int i)
	{
		if (i < 0 || i > SIZE)
		{
			std::cerr << "ERROR: `Shape::operator[] `: index out of bounds\n";
			exit(1);
		}
		else
			return data_[i];
	}

	char operator () (int x, int y)
	{
		if (x > TERM_SIZE_X || y > TERM_SIZE_Y)
		{
			std::cerr << "ERROR: `Shape::operator() `: index out of bounds\n";
			exit(1);
		}
		else
			return data_[y * TERM_SIZE_X + x].getChar();
	}

	/**
	* A function to determine whether a shape contains a given point.
	*
	* Arguments:
	* - `p`: The point we are considering.
	*
	* Returns:
	* - `true` if the given point is inside the shape, `false`
	*   otherwise.
	*/
	virtual bool contains(const Point & p) = 0;
	virtual ~Shape() = default;
};

class Rectangle : public Shape
{
public:
	Rectangle() {}

	Rectangle(int frameNum)
	{
		for (int i = 0; i < 3; i++)
		{
			data_[i * TERM_SIZE_X + frameNum].setChar('*');
			data_[i * TERM_SIZE_X + (7 + frameNum)].setChar('*');
			if (i == 2 || i == 0)
				for (int j = 0; j < 8; j++)
					data_[(i + 1) * TERM_SIZE_X + (j + frameNum)].setChar('*');
		}
	}
	bool contains(const Point & p)
	{
		const int tmp = const_cast<Point&>(p).getY() * TERM_SIZE_X + const_cast<Point&>(p).getX();
		if (data_[tmp].getChar() != ' ')
			return true;
		else
			return false;
	}
};

class Ellipse : public Shape
{
public:
	Ellipse() { }
	Ellipse(int frameNum)
	{
		double x, y;
		int radius = 5;
		for (double radians = 0.1; radians < 2 * 3.141; radians += .1)
		{
			x = radius*cos(radians);
			y = radius*sin(radians);
			data_[((int)(((int)y - 8) * TERM_SIZE_X + (x + frameNum)))*-1].setChar('*');
		}
	}
	bool contains(const Point & p)
	{
		int tmp = const_cast<Point&>(p).getY() * TERM_SIZE_X + const_cast<Point&>(p).getX();
		if (data_[tmp].getChar() != ' ')
			return true;
		else
			return false;
	}
};

class Square : public Rectangle
{
public:
	Square(int frameNum)
	{
		for (int i = 0; i < 3; i++)
		{
			data_[(i + 11) * TERM_SIZE_X + frameNum].setChar('*');
			data_[(i + 11) * TERM_SIZE_X + (5 + frameNum)].setChar('*');
			if (i == 2 || i == 0)
				for (int j = 0; j < 6; j++)
					data_[(i + 11) * TERM_SIZE_X + (j + frameNum)].setChar('*');
		}
	}
};

class Circle : public Ellipse
{
public:
	Circle(int frameNum)
	{
		double x, y;
		int radius = 5;
		for (double radians = 0.1; radians < 2 * 3.141; radians += .01)
		{
			x = (radius * 2)*cos(radians);
			y = (radius)*sin(radians);
			data_[((int)(((int)y - 18) * TERM_SIZE_X + ((int)x + frameNum)))*-1].setChar('*');
		}
	}
};


/**
* A function to draw the `Shape*`s in the given vector in a terminal.
*
* Arguments:
* - `v`: A vector containing pointers to each `Shape` to draw.
*
* Notes:
* - A terminal window is typically 80 columns wide by 25 lines high.
* - The width:height aspect ratio of a terminal character is approximately
*   1:1.9.
*/
void draw(const vector<Shape*> & v) {
	char board[SIZE];
	for (int y = 0; y < TERM_SIZE_Y; y++) {
		for (int x = 0; x < TERM_SIZE_X; x++) {
			board[y * TERM_SIZE_X + x] = ' ';
			for (int i = 0; i < v.size(); i++) {
				if (v[i]->contains(Point(x, y)))
					board[y * TERM_SIZE_X + x] = v[i]->operator()(x, y);
			}
			cout << board[y * TERM_SIZE_X + x];
		}
	}
	cout << endl;
}

int main() {
	const int frameSleep = 100;
	const int frameCount = TERM_SIZE_X - 8;

	for (int frames = 0; frames < frameCount; frames++)
	{
		Rectangle r = Rectangle(frames);
		Ellipse e = Ellipse(frames);
		Square s = Square(frames);
		Circle c = Circle(frames);
		vector<Shape*> shapes = { &e, &r, &s, &c };

		draw(shapes);
		std::this_thread::sleep_for(std::chrono::milliseconds(frameSleep));
	}
	return 0;
}
