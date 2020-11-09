#include <SFML/Graphics.hpp>
using namespace sf;

// Window screenWidth and screenHeight
int screenWidth = 1024;
int screenHeight = 768;

// Set screenWidth of road
float roadWidth = 3000.0f;

// Set segment length
float segmentLength = 200.0f;

// Set camera depth
float cameraDepth = 0.84f;

// Function for drawing quads    
void drawQuad(RenderWindow &w, Color c, float x1, float y1, float w1, float x2, float y2, float w2)
{
	ConvexShape shape(4);
	shape.setFillColor(c);
	shape.setPoint(0, Vector2f(x1 - w1, y1));
	shape.setPoint(1, Vector2f(x2 - w2, y2));
	shape.setPoint(2, Vector2f(x2 + w2, y2));
	shape.setPoint(3, Vector2f(x1 + w1, y1));
	w.draw(shape);
}

struct Line
{
	// 3D centre of line
	float x, y, z;

	// Screen coordinates
	float X, Y, W;
	float curve, clip, scale;

	Line()
	{
		curve = x = y = z = 0;
	}

	// Project road onto the screen. 
	void project(float camX, float camY, float camZ)
	{
		scale = cameraDepth / (z - camZ);
		X = (1 + scale * (x - camX)) * screenWidth / 2;
		Y = (1 - scale * (y - camY)) * screenHeight / 2;
		W = scale * roadWidth  * screenWidth / 2;
	}

};


int main()
{
	RenderWindow app(VideoMode(screenWidth, screenHeight), "2D Racer");

	// Set permanent frame rate
	app.setFramerateLimit(60);

	int H = 1000;
	// Load background texture
	Texture bg;
	bg.loadFromFile("images/bg.png");
	bg.setRepeated(true);
	Sprite sBackground(bg);
	sBackground.setTextureRect(IntRect(0, 0, 5000, 411));
	sBackground.setPosition(-2000, 0);

	std::vector<Line> lines;
	// Track Length
	for (int i = 0; i<1600; i++)
	{
		//Set track twist and loops throughout the track length. 
		Line line;
		line.z = i * segmentLength;
		if (i>100 && i<700) line.curve = 0.5f;
		if (i>750) line.y = sin(i / 30.0f) * H;
		if (i>1100) line.curve = -0.7f;
		if (i>1400) line.curve = 2.0f;

		lines.push_back(line);
	}

	int N = lines.size();
	float playerX = 0.0f;
	float pos = 0.0f;


	while (app.isOpen())
	{
		Event e;
		while (app.pollEvent(e))
		{
			if (e.type == Event::Closed)
				app.close();
		}

		float speed = 0.0f;

		if (Keyboard::isKeyPressed(Keyboard::Right)) playerX += 0.2f;
		if (Keyboard::isKeyPressed(Keyboard::Left)) playerX -= 0.2f;
		if (Keyboard::isKeyPressed(Keyboard::Up)) speed = 1000;
		if (Keyboard::isKeyPressed(Keyboard::Down)) speed = -1000;
		if (Keyboard::isKeyPressed(Keyboard::Space)) speed *= 3;
		if (Keyboard::isKeyPressed(Keyboard::Q)) H += 100;
		if (Keyboard::isKeyPressed(Keyboard::A)) H -= 100;

		pos += speed;
		while (pos >= N * segmentLength) pos -= N * segmentLength;
		while (pos < 0) pos += N * segmentLength;

		app.clear(Color(105, 205, 4));
		app.draw(sBackground);
		float startPos = pos / segmentLength;
		float camH = lines[startPos].y + H;
		if (speed>0) sBackground.move(-lines[startPos].curve * 2.0f, 0.0f);
		if (speed<0) sBackground.move(lines[startPos].curve * 2.0f, 0.0f);

		float maxy = screenHeight;
		float x = 0.0f, dx = 0.0f;

		// Draw road to the screen 
		for (int n = startPos; n<startPos + 300; n++)
		{
			Line &l = lines[n%N];
			l.project(playerX*roadWidth - x, camH, startPos*segmentLength - (n >= N ? N * segmentLength : 0));
			x += dx;
			dx += l.curve;

			l.clip = maxy;
			if (l.Y >= maxy) continue;
			maxy = l.Y;

			Color grass = (n / 3) % 2 ? Color(16, 200, 16) : Color(0, 154, 0);
			Color rumble = (n / 3) % 2 ? Color(255, 255, 255) : Color(255, 0, 0);
			Color road = (n / 3) % 2 ? Color(110, 110, 110) : Color(105, 105, 105);

			Line p = lines[(n - 1) % N]; //previous line

			drawQuad(app, grass, 0, p.Y, screenWidth, 0, l.Y, screenWidth);
			drawQuad(app, rumble, p.X, p.Y, p.W*1.2, l.X, l.Y, l.W*1.2);
			drawQuad(app, road, p.X, p.Y, p.W, l.X, l.Y, l.W);
		}

		app.display();
	}

	return 0;
}