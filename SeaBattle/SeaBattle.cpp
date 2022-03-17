#include <SFML/Graphics.hpp>
#include <cstring>
#include <vector>
#include <cmath>

using namespace sf;
using namespace std;

vector<VideoMode> getWindowSize()
{
	vector<VideoMode> v = VideoMode::getFullscreenModes();
	return v;
}

const int N = 10;
const int M = 10;
const Vector2f centerPos(getWindowSize()[0].width / 2, getWindowSize()[0].height / 2);
const Vector2f sectionSize(40, 40);
RectangleShape MisshitSquare(Vector2f(sectionSize.x - 4, sectionSize.y - 4));
RectangleShape hitSquare(Vector2f(sectionSize.x - 4, sectionSize.y - 4));

struct field
{
	Vector2f start;
	Vector2f close;
	bool hit = false;
	bool ship = false;
};

struct ship
{
	int deckQuantity = 0;
	int hitPoint = 0;
	RectangleShape boat;
};

struct placementOutput
{
	RectangleShape ship;
	Text textCounter;
	int counter = 0;
};

struct continueInform
{
	Vector2i savedPos = { -1, -1 };
	Vector2i savedNumberShip = { -1, -1 };
	vector <int> direction = { 1, 2, 3, 4 };
	int directionNumber;
	bool successDestruction = true;
};

void customizationOfText(Text& text, Font& font, int characterSize, wstring str, Vector2f pos, Color color)
{
	text.setFont(font);
	text.setCharacterSize(characterSize);
	text.setString(str);
	text.setFillColor(color);
	text.setPosition(pos);
}

void drawCoords(RenderWindow& window, Vector2f start, Font& font)
{
	const string letterCoord = "ABCDEFGHIJ";
	Text coordinate(letterCoord[0], font, 20);
	coordinate.setFillColor(Color::Black);
	for (int i = 0; i < 10; i++)
	{
		coordinate.setString(letterCoord[i]);
		coordinate.setPosition(start.x - sectionSize.x + 10, start.y + sectionSize.y * i + 10);
		window.draw(coordinate);
		coordinate.setString(to_string(i + 1));
		coordinate.setPosition(start.x + sectionSize.x * i + 10, start.y - sectionSize.y + 5);
		window.draw(coordinate);
	}
}

void drawGrid(RenderWindow& window)
{
	RectangleShape line(Vector2f(getWindowSize()[0].width, 2));
	line.setFillColor(Color(154, 237, 229, 255));
	for (int i = 0; i < getWindowSize()[0].height; i += 40)
	{
		line.setPosition(0, i);
		window.draw(line);
	}
	line.setRotation(90);
	for (int i = 0; i < getWindowSize()[0].width; i += 40)
	{
		line.setPosition(i, 0);
		window.draw(line);
	}
}

void destructionWhenRotating(RectangleShape& destructionField)
{
	int rotation = destructionField.getRotation();
	switch (rotation % 360)
	{
	case 90:
	{
		destructionField.setPosition(destructionField.getPosition().x + 2 * sectionSize.x, destructionField.getPosition().y);
		break;
	}
	case 180:
	{
		destructionField.setPosition(destructionField.getPosition().x + 2 * sectionSize.x, destructionField.getPosition().y + 2 * sectionSize.y);
		break;
	}
	case 270:
	{
		destructionField.setPosition(destructionField.getPosition().x, destructionField.getPosition().y + 2 * sectionSize.y);
		break;
	}
	}
}

void circularDestruction(RenderWindow& window, field battlefield[][M], RectangleShape& tempShip)
{
	RectangleShape destructionField = tempShip;
	destructionField.setSize(Vector2f(tempShip.getSize().x + sectionSize.x * 2, tempShip.getSize().y + sectionSize.y * 2));
	destructionField.setPosition(tempShip.getPosition().x - sectionSize.x, tempShip.getPosition().y - sectionSize.y);
	destructionField.setRotation(tempShip.getRotation());
	if (destructionField.getRotation() != 0)
		destructionWhenRotating(destructionField);

	for (int i = 0; i < N; i++)
		for (int j = 0; j < M; j++)
		{
			Vector2f centerPosSection(battlefield[i][j].start.x + sectionSize.x / 2, battlefield[i][j].start.y + sectionSize.y / 2);
			if (destructionField.getGlobalBounds().contains(centerPosSection))
			{
				battlefield[i][j].hit = true;
			}
		}
}

void assignmentOfShip(field battlefield[][M], RectangleShape& tempShip, int& deckNumber, bool reset)
{
	int deckCounter = 0;
	for (int i = 0; i < N; i++)
		for (int j = 0; j < M; j++)
		{
			Vector2f centerPosSection(battlefield[i][j].start.x + sectionSize.x / 2, battlefield[i][j].start.y + sectionSize.y / 2);
			if (tempShip.getGlobalBounds().contains(centerPosSection))
			{
				if (!reset)
					battlefield[i][j].ship = true;
				else
					battlefield[i][j].ship = false;
				deckCounter++;
				if (deckCounter == deckNumber)
					return;
			}
		}
}

void posWhenRotating(RectangleShape& tempShip, int rotation, Vector2i& sectionQuantity)
{
	switch (rotation % 360)
	{
	case 0:
	{
		tempShip.setPosition(sectionSize.x * sectionQuantity.x, sectionSize.y * sectionQuantity.y + 2);
		break;
	}
	case 90:
	{
		tempShip.setPosition(sectionSize.x * sectionQuantity.x + sectionSize.x - 2, sectionSize.y * sectionQuantity.y + 2);
		break;
	}
	case 180:
	{
		tempShip.setPosition(sectionSize.x * sectionQuantity.x - 2 + sectionSize.x, sectionSize.y * sectionQuantity.y + sectionSize.y);
		break;
	}
	case 270:
	{
		tempShip.setPosition(sectionSize.x * sectionQuantity.x, sectionSize.y * sectionQuantity.y + sectionSize.y);
		break;
	}
	}
}

void posEnvironmentWhenRotating(RectangleShape& shipEnvironment, int rotation, Vector2i& sectionQuantity)
{
	switch (rotation % 360)
	{
	case 0:
	{
		shipEnvironment.setPosition(sectionSize.x * (sectionQuantity.x - 1), sectionSize.y * (sectionQuantity.y - 1) + 2);
		break;
	}
	case 90:
	{
		shipEnvironment.setPosition(sectionSize.x * sectionQuantity.x + 2 * sectionSize.x - 2, sectionSize.y * (sectionQuantity.y - 1) + 2);
		break;
	}
	case 180:
	{
		shipEnvironment.setPosition(sectionSize.x * sectionQuantity.x + 2 * sectionSize.x - 2, sectionSize.y * sectionQuantity.y + 2 * sectionSize.y);
		break;
	}
	case 270:
	{
		shipEnvironment.setPosition(sectionSize.x * (sectionQuantity.x - 1), sectionSize.y * sectionQuantity.y + 2 * sectionSize.y);
		break;
	}
	}
}

bool intersectWithBattleBox(RectangleShape& battleBox, RectangleShape& ship)
{
	Vector2f start(ship.getGlobalBounds().left, ship.getGlobalBounds().top);
	Vector2f close(start.x + ship.getGlobalBounds().width, start.y + ship.getGlobalBounds().height);
	if (battleBox.getGlobalBounds().contains(start) && battleBox.getGlobalBounds().contains(close))
		return true;
	else
		return false;
}

void drawAttackedSections(RenderWindow& window, field battlefield[][M])
{
	for (int i = 0; i < N; i++)
		for (int j = 0; j < M; j++)
		{
			if (battlefield[i][j].hit && battlefield[i][j].ship)
			{
				hitSquare.setPosition(battlefield[i][j].start.x, battlefield[i][j].start.y);
				window.draw(hitSquare);
			}
			if (battlefield[i][j].hit && !battlefield[i][j].ship)
			{
				MisshitSquare.setPosition(battlefield[i][j].start.x, battlefield[i][j].start.y);
				window.draw(MisshitSquare);
			}
		}
}

bool checkShipsNear(RectangleShape& shipEnvironment, field battlefield[][M])
{
	for (int i = 0; i < N; i++)
		for (int j = 0; j < M; j++)
		{
			Vector2f centerPosSection(battlefield[i][j].start.x + sectionSize.x / 2, battlefield[i][j].start.y + sectionSize.y / 2);
			if (shipEnvironment.getGlobalBounds().contains(centerPosSection) && battlefield[i][j].ship)
			{
				return true;
			}
		}
	return false;
}

void randPlace(RenderWindow& window, vector <vector<ship>>& fleet, field battlefield[][M], RectangleShape& battleBox)
{
	Vector2f startBox(battleBox.getPosition());
	for (int i = fleet.size() - 1; i >= 0; i--)
		for (int j = 0; j < 4 - i; j++)
		{
			ship tempShip;
			tempShip.boat.setSize(Vector2f(sectionSize.x * (i + 1) - 2, sectionSize.y - 2));
			tempShip.deckQuantity = i + 1;
			tempShip.hitPoint = i + 1;

			RectangleShape shipEnvironment(Vector2f(tempShip.boat.getSize().x + sectionSize.x * 2, tempShip.boat.getSize().y + sectionSize.y * 2));

			while (!intersectWithBattleBox(battleBox, tempShip.boat))
			{
			replace:
				tempShip.boat.setRotation((90 * (rand() % 5)) % 360);
				shipEnvironment.setRotation(tempShip.boat.getRotation());
				tempShip.boat.setPosition(startBox.x + sectionSize.x * (rand() % 10) + 2, startBox.y + sectionSize.y * (rand() % 10) + 2);
				Vector2i sectionQuantity(tempShip.boat.getGlobalBounds().left / sectionSize.x, tempShip.boat.getGlobalBounds().top / sectionSize.y);
				posWhenRotating(tempShip.boat, tempShip.boat.getRotation(), sectionQuantity);
				posEnvironmentWhenRotating(shipEnvironment, shipEnvironment.getRotation(), sectionQuantity);

				if (checkShipsNear(shipEnvironment, battlefield))
					goto replace;
			}
			assignmentOfShip(battlefield, tempShip.boat, tempShip.deckQuantity, false);
			fleet[i].push_back(tempShip);
		}

}

bool placeShip(RenderWindow& window, vector <vector<ship>>& fleet, field battlefield[][M], RectangleShape& battleBox)
{
	Font font;
	font.loadFromFile("times-new-roman.ttf");
	vector <placementOutput> forPlacement;
	Vector2f placementStart(sectionSize.x, sectionSize.y * 8);

	int deckNumber;
	int rotation = 0;
	int shipsOnBattleField = 0;

	RectangleShape tempShip;
	RectangleShape shipEnvironment;
	shipEnvironment.setFillColor(Color(0, 0, 0, 0));

	bool selectShip = false;
	ship shipForPush;

	if (battleBox.getPosition().x > centerPos.x)
		placementStart = Vector2f(battleBox.getPosition().x + battleBox.getGlobalBounds().width + sectionSize.x - 1, sectionSize.y * 8);

	for (int i = 0; i < 4; i++)
	{
		placementOutput forPush;
		RectangleShape boat;
		boat.setSize(Vector2f(sectionSize.x * (i + 1) - 2, sectionSize.y - 2));
		boat.setFillColor(Color::Green);
		forPush.ship = boat;
		forPush.ship.setPosition(placementStart.x, placementStart.y + sectionSize.y * ((i + 1) * 2) + 2);
		forPush.counter = 4 - i;
		customizationOfText(forPush.textCounter, font, 20, L"x" + to_string(forPush.counter), Vector2f(placementStart.x + sectionSize.x * 4 + 10, placementStart.y + sectionSize.y * ((i + 1) * 2) + 10), Color::Black);
		forPlacement.push_back(forPush);
	}

	RectangleShape enter = forPlacement[3].ship;
	enter.setFillColor(Color::Red);
	enter.setOutlineThickness(2);
	enter.setOutlineColor(Color::Black);
	enter.setPosition(battleBox.getPosition().x + (double)sectionSize.x * N * 0.3, battleBox.getPosition().y + (double)sectionSize.y * M * 1.1);

	Text save;
	customizationOfText(save, font, 20, L"Сохранить", Vector2f(enter.getPosition().x + 30, enter.getPosition().y + 5), Color::Black);

	while (1)
	{
		Event event;
		while (window.pollEvent(event))
		{
			Vector2f mousePos(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
			if (event.type == Event::Closed || (Keyboard::isKeyPressed(Keyboard::Escape) && Keyboard::isKeyPressed(Keyboard::LShift)))
				return false;
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == Mouse::Left)
				{
					if (selectShip && intersectWithBattleBox(battleBox, tempShip) && !checkShipsNear(shipEnvironment, battlefield))
					{
						forPlacement[deckNumber].counter--;
						forPlacement[deckNumber].textCounter.setString(L"x" + to_string(forPlacement[deckNumber].counter));

						shipForPush.boat = tempShip;
						shipForPush.deckQuantity = deckNumber + 1;
						shipForPush.hitPoint = deckNumber + 1;

						fleet[deckNumber].push_back(shipForPush);
						shipsOnBattleField++;
						assignmentOfShip(battlefield, tempShip, shipForPush.deckQuantity, false);

						rotation = 0;
						mousePos = Vector2f(0, 0);
						selectShip = false;
					}
					if (!battleBox.getGlobalBounds().contains(mousePos) && selectShip)
					{
						tempShip.setSize(Vector2f(0, 0));
						selectShip = false;
					}
					if (!selectShip)
					{
						for (int i = 0; i < forPlacement.size(); i++)
							if (forPlacement[i].ship.getGlobalBounds().contains(mousePos) && forPlacement[i].counter > 0)
							{
								tempShip = forPlacement[i].ship;
								deckNumber = i;
								selectShip = true;

								shipEnvironment = tempShip;
								shipEnvironment.setSize(Vector2f(tempShip.getSize().x + sectionSize.x * 2, tempShip.getSize().y + sectionSize.y * 2));
								shipEnvironment.setPosition(tempShip.getPosition().x - sectionSize.x, tempShip.getPosition().y - sectionSize.y);
							}
						for (int i = 0; i < fleet.size(); i++)
							for (int j = 0; j < fleet[i].size(); j++)
								if (fleet[i][j].boat.getGlobalBounds().contains(mousePos))
								{
									assignmentOfShip(battlefield, fleet[i][j].boat, fleet[i][j].deckQuantity, true);
									tempShip = fleet[i][j].boat;

									shipEnvironment = tempShip;
									shipEnvironment.setSize(Vector2f(tempShip.getSize().x + sectionSize.x * 2, tempShip.getSize().y + sectionSize.y * 2));
									shipEnvironment.setRotation(tempShip.getRotation());
									Vector2i sectionQuantity(mousePos.x / sectionSize.x, mousePos.y / sectionSize.y);
									posEnvironmentWhenRotating(shipEnvironment, tempShip.getRotation(), sectionQuantity);

									deckNumber = fleet[i][j].deckQuantity - 1;
									forPlacement[fleet[i][j].deckQuantity - 1].counter++;
									forPlacement[fleet[i][j].deckQuantity - 1].textCounter.setString(L"x" + to_string(forPlacement[fleet[i][j].deckQuantity - 1].counter));
									fleet[i].erase(fleet[i].begin() + j);
									shipsOnBattleField--;
									selectShip = true;
								}
					}
					if (shipsOnBattleField == 10)
						enter.setFillColor(Color::Green);
					else
						enter.setFillColor(Color::Red);
					if (enter.getGlobalBounds().contains(mousePos) && shipsOnBattleField == 10)
						return true;
				}
			}
			if (event.type == sf::Event::MouseWheelScrolled)
			{
				rotation += 90;
				tempShip.setRotation(rotation % 360);
				shipEnvironment.setRotation(rotation % 360);

				Vector2i sectionQuantity(mousePos.x / sectionSize.x, mousePos.y / sectionSize.y);
				posWhenRotating(tempShip, rotation, sectionQuantity);
				posEnvironmentWhenRotating(shipEnvironment, rotation, sectionQuantity);
			}
			if (event.type == Event::MouseMoved && selectShip)
			{
				if (checkShipsNear(shipEnvironment, battlefield))
					tempShip.setFillColor(Color::Red);
				else
					tempShip.setFillColor(Color::Green);
				Vector2i sectionQuantity(mousePos.x / sectionSize.x, mousePos.y / sectionSize.y);
				posWhenRotating(tempShip, tempShip.getRotation(), sectionQuantity);
				posEnvironmentWhenRotating(shipEnvironment, tempShip.getRotation(), sectionQuantity);
			}
		}
		window.clear(Color::White);

		for (int i = 0; i < forPlacement.size(); i++)
		{
			window.draw(forPlacement[i].ship);
			window.draw(forPlacement[i].textCounter);
		}

		for (int i = 0; i < fleet.size(); i++)
			for (int j = 0; j < fleet[i].size(); j++)
				window.draw(fleet[i][j].boat);

		window.draw(tempShip);
		drawGrid(window);
		window.draw(enter);
		window.draw(save);
		window.draw(battleBox);
		drawCoords(window, battleBox.getPosition(), font);

		window.display();
		sleep(milliseconds(1000 / 60));
	}
}

void endGame(RenderWindow& window, int& totalHitPointsLeft)
{
	Font font;
	font.loadFromFile("times-new-roman.ttf");
	Text endStroke(L"Капитан левого флота победил", font, 40);
	endStroke.setFillColor(Color::Red);
	endStroke.setPosition(centerPos.x - endStroke.getGlobalBounds().width / 2, sectionSize.y * 5 - 5);

	Text exitStroke(L"Завершить игру", font, 25);
	exitStroke.setFillColor(Color::Black);

	RectangleShape exitRect(Vector2f(sectionSize.x * 6 - 4, sectionSize.y - 4));
	exitRect.setPosition(centerPos.x - exitRect.getGlobalBounds().width / 2, sectionSize.y * 20 + 2);
	exitRect.setFillColor(Color::Red);
	exitRect.setOutlineThickness(2);
	exitRect.setOutlineColor(Color::Black);
	exitStroke.setPosition(centerPos.x - exitStroke.getGlobalBounds().width / 2, exitRect.getPosition().y + exitRect.getGlobalBounds().height / 2 - exitStroke.getGlobalBounds().height);

	if (totalHitPointsLeft == 0)
		endStroke.setString(L"Капитан правого флота победил");

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			Vector2f mousePos(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
			if (event.type == Event::Closed || (Keyboard::isKeyPressed(Keyboard::Escape) && Keyboard::isKeyPressed(Keyboard::LShift)))
				window.close();
			if (event.type == Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == Mouse::Left)
				{
					if (exitRect.getGlobalBounds().contains(mousePos))
						return;
				}
			}
			if (event.type == Event::MouseMoved)
			{
				if (exitRect.getGlobalBounds().contains(mousePos))
					exitRect.setFillColor(Color::Green);
				else
					exitRect.setFillColor(Color::Red);
			}
		}
		window.draw(endStroke);
		window.draw(exitRect);
		window.draw(exitStroke);
		window.display();
		sleep(milliseconds(1000 / 60));
	}
}

field& findSection(field battlefield[][M], Vector2f& mousePos)
{
	field empty;
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < M; j++)
			if (mousePos.x > battlefield[i][j].start.x && mousePos.x < battlefield[i][j].close.x && mousePos.y > battlefield[i][j].start.y && mousePos.y < battlefield[i][j].close.y)
			{
				return battlefield[i][j];
			}
	}
	return empty;
}

bool attackDirection(int& direction, field humanBattlefield[][M], Vector2i& startAttack, int& hitCombo, int& HumanTotalHitPoints, bool& cancel)
{
	bool shipAttacked = false;
	switch (direction)
	{
	case 1:
	{
		if (!humanBattlefield[startAttack.x][startAttack.y - hitCombo].hit && startAttack.y - hitCombo > -1)
			humanBattlefield[startAttack.x][startAttack.y - hitCombo].hit = true;
		else
		{
			cancel = true;
			break;
		}
		if (humanBattlefield[startAttack.x][startAttack.y - hitCombo].ship)
			shipAttacked = true;
		break;
	}
	case 2:
	{
		if (!humanBattlefield[startAttack.x + hitCombo][startAttack.y].hit && startAttack.x + hitCombo < 10)
			humanBattlefield[startAttack.x + hitCombo][startAttack.y].hit = true;
		else
		{
			cancel = true;
			break;
		}
		if (humanBattlefield[startAttack.x + hitCombo][startAttack.y].ship)
			shipAttacked = true;
		break;
	}
	case 3:
	{
		if (!humanBattlefield[startAttack.x][startAttack.y + hitCombo].hit && startAttack.y + hitCombo < 10)
			humanBattlefield[startAttack.x][startAttack.y + hitCombo].hit = true;
		else
		{
			cancel = true;
			break;
		}
		if (humanBattlefield[startAttack.x][startAttack.y + hitCombo].ship)
			shipAttacked = true;
		break;
	}
	case 4:
	{
		if (!humanBattlefield[startAttack.x - hitCombo][startAttack.y].hit && startAttack.x - hitCombo > -1)
			humanBattlefield[startAttack.x - hitCombo][startAttack.y].hit = true;
		else
		{
			cancel = true;
			break;
		}
		if (humanBattlefield[startAttack.x - hitCombo][startAttack.y].ship)
			shipAttacked = true;
		break;
	}
	}
	sleep(milliseconds(500));
	if (shipAttacked)
		HumanTotalHitPoints--;
	return shipAttacked;
}

void finalDestructionWay(RenderWindow& window, vector <vector<ship>>& humanFleet, field humanBattlefield[][M], continueInform& dataOfBot, int directionNumber, int& HumanTotalHitPoints)
{
	int hitCombo = 2;
	bool cancel = false;

	if (dataOfBot.successDestruction)
	{
		humanFleet[dataOfBot.savedNumberShip.x][dataOfBot.savedNumberShip.y].hitPoint--;
		drawAttackedSections(window, humanBattlefield);
		window.display();
		sleep(milliseconds(500));
	}
	else
		hitCombo = 1;
	while (humanFleet[dataOfBot.savedNumberShip.x][dataOfBot.savedNumberShip.y].hitPoint > 0)
	{
		if (!attackDirection(dataOfBot.direction[directionNumber], humanBattlefield, dataOfBot.savedPos, hitCombo, HumanTotalHitPoints, cancel) && humanFleet[dataOfBot.savedNumberShip.x][dataOfBot.savedNumberShip.y].hitPoint != 0)
		{
			dataOfBot.successDestruction = false;
			int tmp = (dataOfBot.direction[directionNumber] + 2) % 4;
			dataOfBot.direction.clear();
			dataOfBot.direction.push_back(tmp);
			if (cancel)
			{
				finalDestructionWay(window, humanFleet, humanBattlefield, dataOfBot, 0, HumanTotalHitPoints);
			}
			return;
		}
		humanFleet[dataOfBot.savedNumberShip.x][dataOfBot.savedNumberShip.y].hitPoint--;
		hitCombo++;
		drawAttackedSections(window, humanBattlefield);
		window.display();
		if (humanFleet[dataOfBot.savedNumberShip.x][dataOfBot.savedNumberShip.y].hitPoint != 0)
			sleep(milliseconds(500));
	}
	circularDestruction(window, humanBattlefield, humanFleet[dataOfBot.savedNumberShip.x][dataOfBot.savedNumberShip.y].boat);
	dataOfBot.successDestruction = true;
	dataOfBot.direction = { 1, 2, 3, 4 };
	dataOfBot.savedPos = Vector2i(-1, -1);
	dataOfBot.savedNumberShip = Vector2i(-1, -1);
}

void botStep(RenderWindow& window, vector <vector<ship>>& humanFleet, Sprite& arrow, bool& leftStep, Texture& arrowOrientation, field humanBattlefield[][M], continueInform& dataOfBot, int& HumanTotalHitPoints)
{
	int hitCombo = 1;
	bool cancel = false;
	int directionNumber;
	bool CheckShip;
	if (dataOfBot.direction.size() < 4)
	{
		if (dataOfBot.successDestruction)
		{
			directionNumber = rand() % dataOfBot.direction.size();
			CheckShip = attackDirection(dataOfBot.direction[directionNumber], humanBattlefield, dataOfBot.savedPos, hitCombo, HumanTotalHitPoints, cancel);
			while (cancel)
			{
				cancel = false;
				dataOfBot.direction.erase(dataOfBot.direction.begin() + directionNumber);
				directionNumber = rand() % dataOfBot.direction.size();
				CheckShip = attackDirection(dataOfBot.direction[directionNumber], humanBattlefield, dataOfBot.savedPos, hitCombo, HumanTotalHitPoints, cancel);
			}
		}
		else
		{
			directionNumber = 0;
			CheckShip = true;
		}

		if (!CheckShip)
		{
			dataOfBot.direction.erase(dataOfBot.direction.begin() + directionNumber);
			leftStep = !leftStep;
			arrow.setTexture(arrowOrientation);
			return;
		}
		else
		{
			finalDestructionWay(window, humanFleet, humanBattlefield, dataOfBot, directionNumber, HumanTotalHitPoints);
			if (!dataOfBot.successDestruction)
			{
				leftStep = !leftStep;
				arrow.setTexture(arrowOrientation);
				return;
			}
			sleep(milliseconds(500));
		}
	}
	if (dataOfBot.savedPos.x == -1)
	{
		dataOfBot.savedPos.x = rand() % 10;
		dataOfBot.savedPos.y = rand() % 10;
		while (humanBattlefield[dataOfBot.savedPos.x][dataOfBot.savedPos.y].hit)
		{
			dataOfBot.savedPos.x = rand() % 10;
			dataOfBot.savedPos.y = rand() % 10;
		}
	}
	int i = dataOfBot.savedPos.x, j = dataOfBot.savedPos.y;
	if (!humanBattlefield[i][j].hit)
	{
		humanBattlefield[i][j].hit = true;
		drawAttackedSections(window, humanBattlefield);
		sleep(milliseconds(500));
		window.display();
		if (humanBattlefield[i][j].ship)
		{
			int k, m;
			for (k = 0; k < humanFleet.size(); k++)
				for (m = 0; m < humanFleet[k].size(); m++)
				{
					if (humanFleet[k][m].boat.getGlobalBounds().contains(humanBattlefield[i][j].start))
					{
						humanFleet[k][m].hitPoint--;
						HumanTotalHitPoints--;
						dataOfBot.savedNumberShip = Vector2i(k, m);
						if (humanFleet[k][m].hitPoint == 0)
							circularDestruction(window, humanBattlefield, humanFleet[k][m].boat);
					}
				}
			if (humanFleet[dataOfBot.savedNumberShip.x][dataOfBot.savedNumberShip.y].deckQuantity != 1)
			{
				dataOfBot.savedPos = Vector2i(i, j);
				directionNumber = rand() % dataOfBot.direction.size();
				hitCombo = 1;
				cancel = false;
				CheckShip = attackDirection(dataOfBot.direction[directionNumber], humanBattlefield, dataOfBot.savedPos, hitCombo, HumanTotalHitPoints, cancel);
				while (cancel)
				{
					cancel = false;
					dataOfBot.direction.erase(dataOfBot.direction.begin() + directionNumber);
					directionNumber = rand() % dataOfBot.direction.size();
					CheckShip = attackDirection(dataOfBot.direction[directionNumber], humanBattlefield, dataOfBot.savedPos, hitCombo, HumanTotalHitPoints, cancel);
				}
				if (!CheckShip)
				{
					dataOfBot.direction.erase(dataOfBot.direction.begin() + directionNumber);
					dataOfBot.savedPos = Vector2i(i, j);
					leftStep = !leftStep;
					arrow.setTexture(arrowOrientation);
					return;
				}
				else
				{
					finalDestructionWay(window, humanFleet, humanBattlefield, dataOfBot, directionNumber, HumanTotalHitPoints);
					if (!dataOfBot.successDestruction)
					{
						dataOfBot.directionNumber = (directionNumber + 2) % 4;
						leftStep = !leftStep;
						arrow.setTexture(arrowOrientation);
					}
					return;
				}

			}
			else
			{
				dataOfBot.savedPos = Vector2i(-1, -1);
				return;
			}
		}
	}
	dataOfBot.savedPos = Vector2i(-1, -1);
	dataOfBot.savedNumberShip = Vector2i(-1, -1);
	leftStep = !leftStep;
	arrow.setTexture(arrowOrientation);
}

void step(RenderWindow& window, field& sector, Vector2f& mousePos, vector <vector<ship>>& fleet, Sprite& arrow, bool& leftStep, Texture& arrowOrientation, field battlefield[][M], int& totalHitPoints)
{
	if (sector.start == Vector2f(NULL, NULL))
		return;
	int i, j;
	bool exit = false;

	if (sector.hit)
		return;
	if (sector.ship)
	{
		for (int k = 0; k < fleet.size(); k++)
		{
			for (int m = 0; m < fleet[k].size(); m++)
			{
				if (fleet[k][m].boat.getGlobalBounds().contains(mousePos))
				{
					fleet[k][m].hitPoint--;
					totalHitPoints--;
					if (fleet[k][m].hitPoint == 0)
					{
						circularDestruction(window, battlefield, fleet[k][m].boat);
						fleet[k].erase(fleet[k].begin() + m);
					}
					exit = true;
					break;
				}
			}
			if (exit)
				break;
		}
	}
	sector.hit = true;
	if (exit)
		return;

	leftStep = !leftStep;
	arrow.setTexture(arrowOrientation);
}

void aboutPage(RenderWindow& window)
{
	Texture background;
	background.loadFromFile("background.jpeg");
	Sprite about(background);
	while (1)
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == Mouse::Left)
				{
					return;
				}
			}
		}
		window.clear(Color::White);
		window.draw(about);
		window.display();
		sleep(milliseconds(1000 / 60));
	}
}

int selectGameMode(RenderWindow& window, vector <RectangleShape>& buttons, vector <Text> buttonStorkes)
{
	buttonStorkes[0].setString(L"Игрок VS Игрок");
	buttonStorkes[0].setPosition(centerPos.x - buttonStorkes[0].getGlobalBounds().width / 2, buttons[0].getPosition().y + buttons[0].getGlobalBounds().height / 2 - buttonStorkes[0].getGlobalBounds().height);
	buttonStorkes[1].setString(L"Игрок VS Бот");
	buttonStorkes[1].setPosition(centerPos.x - buttonStorkes[1].getGlobalBounds().width / 2, buttons[1].getPosition().y + buttons[1].getGlobalBounds().height / 2 - buttonStorkes[1].getGlobalBounds().height);
	buttonStorkes[2].setString(L"Назад");
	buttonStorkes[2].setPosition(centerPos.x - buttonStorkes[2].getGlobalBounds().width / 2, buttons[2].getPosition().y + buttons[2].getGlobalBounds().height / 2 - buttonStorkes[2].getGlobalBounds().height);
	while (1)
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == Mouse::Left)
				{
					Vector2f mousePos(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
					for (int i = 0; i < 3; i++)
					{
						if (buttons[i].getGlobalBounds().contains(mousePos))
						{
							switch (i)
							{
							case 0:
							{
								return 0;
							}
							case 1:
							{
								return 1;
							}
							case 2:
								return -1;
							}
						}
					}
				}
			}
			if (event.type == Event::MouseMoved)
			{
				Vector2f mousePos(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
				for (int i = 0; i < 3; i++)
				{
					if (buttons[i].getGlobalBounds().contains(mousePos))
						buttons[i].setFillColor(Color::Green);
					else
						buttons[i].setFillColor(Color::Blue);
				}
			}
		}
		window.clear(Color::White);
		drawGrid(window);
		for (int i = 0; i < 3; i++)
		{
			window.draw(buttons[i]);
			window.draw(buttonStorkes[i]);
		}
		window.display();
		sleep(milliseconds(1000 / 60));
	}
}

int preStart(RenderWindow& window)
{
	Font font;
	font.loadFromFile("times-new-roman.ttf");
	wstring strokes[3] = { L"Начать игру" , L"О программе" , L"Выход" };
	vector <RectangleShape> buttons;
	vector <Text> buttonStorkes;
	int indent = 0;
	for (int i = 0; i < 3; i++)
	{
		RectangleShape button(Vector2f(sectionSize.x * 10 - 4, sectionSize.y * 3 - 4));
		button.setPosition(centerPos.x - button.getGlobalBounds().width / 2, sectionSize.y * (8 + i * 3) + sectionSize.y * indent + 2);
		button.setFillColor(Color::Blue);
		button.setOutlineThickness(2);
		button.setOutlineColor(Color::Black);
		indent++;

		Text stroke(strokes[i], font, 40);
		stroke.setPosition(centerPos.x - stroke.getGlobalBounds().width / 2, button.getPosition().y + button.getGlobalBounds().height / 2 - stroke.getGlobalBounds().height);
		buttons.push_back(button);
		buttonStorkes.push_back(stroke);
	}
	while (1)
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == Mouse::Left)
				{
					Vector2f mousePos(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
					for (int i = 0; i < 3; i++)
					{
						if (buttons[i].getGlobalBounds().contains(mousePos))
						{
							switch (i)
							{
							case 0:
							{
								int exodus = selectGameMode(window, buttons, buttonStorkes);
								if (exodus != -1)
									return exodus;
								break;
							}
							case 1:
							{
								aboutPage(window);
								break;
							}
							case 2:
								window.close();;
							}
						}
					}
				}
			}
			if (event.type == Event::MouseMoved)
			{
				Vector2f mousePos(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
				for (int i = 0; i < 3; i++)
				{
					if (buttons[i].getGlobalBounds().contains(mousePos))
						buttons[i].setFillColor(Color::Green);
					else
						buttons[i].setFillColor(Color::Blue);
				}
			}
		}
		window.clear(Color::White);
		drawGrid(window);
		for (int i = 0; i < 3; i++)
		{
			window.draw(buttons[i]);
			window.draw(buttonStorkes[i]);
		}
		window.display();
		sleep(milliseconds(1000 / 60));
	}
}

int main()
{
	ContextSettings settings;
	settings.antialiasingLevel = 8;

	RenderWindow window(VideoMode(getWindowSize()[0]), "SeaBattle", Style::Fullscreen, settings);
start:
	bool withBot = preStart(window);

	Vector2f startLeft(sectionSize.x * 7 - 1, sectionSize.y * 8 + 1);
	Vector2f startRight(getWindowSize()[0].width - (sectionSize.x * (7 + N)) - 1, sectionSize.y * 8 + 1);

	RectangleShape battleBoxLeft(Vector2f(N * sectionSize.x - 4, M * sectionSize.y - 4));
	battleBoxLeft.setPosition(startLeft.x + 2, startLeft.y + 2);
	battleBoxLeft.setFillColor(Color(0, 0, 0, 0));
	battleBoxLeft.setOutlineThickness(2);
	battleBoxLeft.setOutlineColor(Color::Blue);


	RectangleShape battleBoxRight = battleBoxLeft;
	battleBoxRight.setPosition(startRight.x + 2, startRight.y + 2);

	int totalHitPointsLeft = 20;
	int totalHitPointsRight = 20;

	field battlefieldLeft[N][M];
	field battlefieldRight[N][M];
	for (int i = 0; i < N; i++)
		for (int j = 0; j < M; j++)
		{
			Vector2f leftSectionPos(startLeft.x + (sectionSize.x * j) + 2, startLeft.y + (sectionSize.x * i) + 2);
			Vector2f rightSectionPos(startRight.x + (sectionSize.x * j) + 2, startRight.y + (sectionSize.x * i) + 2);

			battlefieldLeft[i][j].start = leftSectionPos;
			battlefieldLeft[i][j].close = Vector2f(leftSectionPos.x + sectionSize.x - 2, leftSectionPos.y + sectionSize.y - 2);

			battlefieldRight[i][j].start = rightSectionPos;
			battlefieldRight[i][j].close = Vector2f(rightSectionPos.x + sectionSize.x - 2, rightSectionPos.y + sectionSize.y - 2);
		}

	Texture left, right, xMarkMiss, xMarkHit;
	left.loadFromFile("left.png");
	right.loadFromFile("right.png");
	xMarkMiss.loadFromFile("xMarkMiss.png");
	xMarkHit.loadFromFile("xMarkHit.png");

	MisshitSquare.setTexture(&xMarkMiss);
	hitSquare.setTexture(&xMarkHit);

	Sprite arrow;
	arrow.setTexture(right);
	arrow.setScale(0.25, 0.25);
	arrow.setPosition(centerPos.x - arrow.getGlobalBounds().width / 2, centerPos.y - arrow.getGlobalBounds().height / 2);
	bool leftStep = true;

	vector <vector<ship>> fleetLeft(4);
	vector <vector<ship>> fleetRight(4);
	vector <vector<ship>> botFleet(4);

	Font font;
	font.loadFromFile("times-new-roman.ttf");
	srand(time(NULL));

	continueInform dataOfBot;

	if (!withBot)
	{
		if (!placeShip(window, fleetLeft, battlefieldLeft, battleBoxLeft) || !placeShip(window, fleetRight, battlefieldRight, battleBoxRight))
			goto start;
	}
	else
	{
		if (!placeShip(window, fleetLeft, battlefieldLeft, battleBoxLeft))
			goto start;
		randPlace(window, botFleet, battlefieldRight, battleBoxRight);
	}

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed || (Keyboard::isKeyPressed(Keyboard::Escape) && Keyboard::isKeyPressed(Keyboard::LShift)))
				goto start;
			if (event.type == Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == Mouse::Left)
				{
					Vector2f mousePos(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
					if (!withBot)
					{
						if (battleBoxLeft.getGlobalBounds().contains(mousePos) && !leftStep)
						{
							step(window, findSection(battlefieldLeft, mousePos), mousePos, fleetLeft, arrow, leftStep, right, battlefieldLeft, totalHitPointsLeft);
						}
						if (battleBoxRight.getGlobalBounds().contains(mousePos) && leftStep)
						{
							step(window, findSection(battlefieldRight, mousePos), mousePos, fleetRight, arrow, leftStep, left, battlefieldRight, totalHitPointsRight);
						}
					}
					else
					{
						if (leftStep)
						{
							step(window, findSection(battlefieldRight, mousePos), mousePos, botFleet, arrow, leftStep, left, battlefieldRight, totalHitPointsRight);
						}
					}
				}
			}

		}
		window.clear(Color::White);
		drawGrid(window);
		window.draw(battleBoxLeft);
		window.draw(battleBoxRight);

		window.draw(arrow);
		drawCoords(window, battleBoxLeft.getPosition(), font);
		drawCoords(window, battleBoxRight.getPosition(), font);

		drawAttackedSections(window, battlefieldLeft);
		drawAttackedSections(window, battlefieldRight);

		if ((totalHitPointsLeft == 0 || totalHitPointsRight == 0))
		{
			endGame(window, totalHitPointsLeft);
			goto start;
		}
		window.display();
		if (withBot && !leftStep)
		{
			sleep(milliseconds(250));
			botStep(window, fleetLeft, arrow, leftStep, right, battlefieldLeft, dataOfBot, totalHitPointsLeft);
		}
		sleep(milliseconds(1000 / 60));
	}
}