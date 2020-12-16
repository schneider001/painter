#include "D:\Txlib\TX\TXlib.h"
#include <stdlib.h>
#include <string>
#include <cmath>
#pragma comment(linker, "/STACK:257772160")

RGBQUAD* Video_memory = NULL;
typedef void (*func_t) (void);
typedef double(*graph_t)(double x);

struct plot {
	double x, y, z;
};




void pencil();
void fill(double x, double y, COLORREF current);
void cover_for_fill();
void draw_point(plot coords, COLORREF color_of_point, double R);

COLORREF color_for_painting = TX_BLACK;
double radius_for_painting = 2;
func_t func_of_drawing = &pencil;

class Window {
public:
	plot coords0_, coords1_;
	virtual void draw_window() {
		txSetColor(TX_GRAY);
		txSetFillColor(TX_WHITE);
		txRectangle(coords0_.x, coords0_.y, coords1_.x, coords1_.y);
	}
	Window(plot coords0, plot coords1) :
		coords0_(coords0),
		coords1_(coords1)
	{}
};

class BasicButton: public Window {
public:
	virtual void draw_button(){
		txSetColor(TX_GRAY);
		txSetFillColor(TX_WHITE);
		txRectangle(coords0_.x, coords0_.y, coords1_.x, coords1_.y);
	}

	virtual bool if_button_pressed() {
		if (txMouseButtons() == 1 &&
			txMouseX() >= coords0_.x &&
			txMouseX() <= coords1_.x &&
			txMouseY() >= coords0_.y &&
			txMouseY() <= coords1_.y) {
			return TRUE;
		}
		else {
			return FALSE;
		}
	}
	virtual void action() {
	}

	BasicButton(plot coords0, plot coords1):
		Window(coords0, coords1)
	{}
};

class SetColorButton: public BasicButton {
public:
	COLORREF color_;
	virtual void draw_button() override {
		txSetColor(TX_GRAY);
		txSetFillColor(color_);
		txRectangle(coords0_.x, coords0_.y, coords1_.x, coords1_.y);
	}
	virtual void action() override {
		color_for_painting = color_;
	}
	SetColorButton(plot coords0, plot coords1, const char* name, COLORREF color):
		BasicButton(coords0, coords1),
		color_(color)
	{}
};

class Scroll_bar : public BasicButton {
public:
	plot coords_of_bar0_;
	plot coords_of_bar1_;
	virtual void draw_button() override {
		txSetColor(TX_LIGHTGRAY, 1);
		txSetFillColor(TX_WHITE);
		txRectangle(coords0_.x, coords0_.y, coords1_.x, coords1_.y);
		coords_of_bar0_ = { (coords1_.x + coords0_.x) / 2 - 7, coords0_.y };
		coords_of_bar1_ = { (coords1_.x + coords0_.x) / 2 + 7, coords1_.y };
		txSetColor(TX_LIGHTGRAY);
		txSetFillColor(TX_LIGHTGRAY);
		txRectangle(coords_of_bar0_.x, coords_of_bar0_.y, coords_of_bar1_.x, coords_of_bar1_.y);
	}

	virtual void action() override {
		double scroll = txMouseX();
		txSetColor(TX_LIGHTGRAY, 1);
		txSetFillColor(TX_WHITE);
		txRectangle(coords0_.x, coords0_.y, coords1_.x, coords1_.y);
		if (scroll > coords0_.x + 7 && scroll < coords1_.x - 7) {
			coords_of_bar0_.x = scroll - 7;
			coords_of_bar1_.x = scroll + 7;
			radius_for_painting = ((coords_of_bar1_.x + coords_of_bar0_.x) / 2 - coords0_.x) / 5 + 1;
			txSetFillColor(TX_LIGHTGRAY);
			txRectangle(coords_of_bar0_.x, coords_of_bar0_.y, coords_of_bar1_.x, coords_of_bar1_.y);
			txSleep(0.1);
		}
	}
	Scroll_bar(plot coords0, plot coords1):
		BasicButton(coords0, coords1)
	{}
};


class FunctionalButton : public BasicButton {
public:
	const char* name_;
	func_t function_;
	virtual void draw_button() override {
		txSetColor(TX_GRAY);
		txSetFillColor(TX_WHITE);
		txRectangle(coords0_.x, coords0_.y, coords1_.x, coords1_.y);
		txDrawText(coords0_.x, coords0_.y, coords1_.x, coords1_.y, name_);
	}
	virtual bool if_button_pressed() override {
		if (txMouseButtons() == 1 &&
			txMouseX() >= coords0_.x &&
			txMouseX() <= coords1_.x &&
			txMouseY() >= coords0_.y &&
			txMouseY() <= coords1_.y) {
			while (txMouseButtons() == 1) {
				txSleep(0.1);
			}
			return TRUE;
		}
		else {
			return FALSE;
		}
	}
	virtual void action() override {
		function_();
	}
	FunctionalButton(plot coords0, plot coords1, const char* name, func_t function):
		BasicButton(coords0, coords1),
		function_(function),
		name_(name)
	{}
};

class InstrumentalButton : public BasicButton {
public:
	const char* name_;
	func_t function_;
	virtual void draw_button() override {
		txSetColor(TX_GRAY);
		txSetFillColor(TX_WHITE);
		txRectangle(coords0_.x, coords0_.y, coords1_.x, coords1_.y);
		txDrawText(coords0_.x, coords0_.y, coords1_.x, coords1_.y, name_);
	}
	virtual void action() override {
		func_of_drawing = function_;
	}
	InstrumentalButton(plot coords0, plot coords1, const char* name, func_t function):
		BasicButton(coords0, coords1),
		function_(function),
		name_(name)
	{}

};

class Canvas : public BasicButton {
public:
	func_t function_;
	virtual void action() override {
		function_();
	}
	Canvas(plot coords0, plot coords1, func_t function):
		BasicButton(coords0, coords1),
		function_(function)
	{}
};

const int Number_of_buttons = 50;
Canvas window_for_painting({ 0, 100 }, { 1500, 900 }, func_of_drawing);
Window window_for_buttons({ 0, 0 }, { 1500, 100 });

class ManagerButton {
public:
	BasicButton* buttons_[Number_of_buttons];

	void add(BasicButton* button) {
		int i;
		for (i = 0; buttons_[i] != NULL; ++i) {}
		buttons_[i] = button;
	}

	void draw_buttons() {
		for (int i = 0; buttons_[i] != NULL; i++) {
			(*buttons_[i]).draw_button();
		}
	}

	void run() {
		while (TRUE) {
			for (int i = 0; buttons_[i] != NULL; i++) {
				if ((*buttons_[i]).if_button_pressed()) {
					(*buttons_[i]).action();
				}
			}
			window_for_painting.function_ = func_of_drawing;
		}
	}

	ManagerButton() {
		for (int i = 0; i < Number_of_buttons; i++) {
			buttons_[i] = NULL;
		}
	}
	~ManagerButton() {
		for (int i = 0; buttons_[i] != NULL; i++) {
			delete buttons_[i];
		}
	}
};


void clear();
void exit_from_program();
void save_image();
void open_image();
void draw_pixel(double x, double y, COLORREF color_of_point);

int main() {
	txCreateWindow(1500, 900);
	window_for_buttons.draw_window();

	ManagerButton manager;

	manager.add(new SetColorButton(  { 0, 0 },   { 25, 25 }, "b", TX_LIGHTBLUE));
	manager.add(new SetColorButton(  { 25, 0 },  { 50, 25 }, "r", TX_LIGHTRED));
	manager.add(new SetColorButton(  { 50, 0 }, { 75, 25 }, "y", TX_YELLOW));
	manager.add(new FunctionalButton({ 200, 0 }, { 250, 50 }, "clear", clear));
	manager.add(new FunctionalButton({ 250,0 },  { 350, 50 }, "exit", exit_from_program));
	manager.add(new FunctionalButton({ 350,0 }, { 450, 50 }, "save", save_image));
	manager.add(new FunctionalButton({ 450,0 }, { 550, 50 }, "open", open_image));
	manager.add(new Scroll_bar({ 0,50 }, { 250, 100 }));
	manager.add(new InstrumentalButton({ 150, 0 }, { 200, 50 }, "p", pencil));
	manager.add(new InstrumentalButton({ 100,0 }, { 150, 50 }, "f", cover_for_fill));
	manager.add(&window_for_painting);


	manager.draw_buttons();

	manager.run();
	manager.~ManagerButton();
}
void clear() {
	window_for_painting.draw_window();
}

void exit_from_program() {
	exit(0);
}

void draw_point(plot coords, COLORREF color_of_point, double R) {
	txSetFillColor(color_of_point);
	txSetColor(color_of_point);
	txCircle(coords.x, coords.y, R);
}

void pencil() {
	plot coords1 = { txMouseX(), txMouseY() };
	while ( coords1.y > window_for_painting.coords0_.y + radius_for_painting) {
		plot coords2 = { txMouseX(), txMouseY() };
		if (txMouseButtons() == 1 && coords2.y > window_for_painting.coords0_.y + radius_for_painting) {
			txSetColor(color_for_painting, radius_for_painting);
			txLine(coords1.x, coords1.y, coords2.x, coords2.y);
			txSleep();
		}
		coords1 = coords2;
	}
}

void save_image() {
	bool is_image_saved = txSaveImage("image.bmp");
}

void open_image() {
	HDC source_image = txLoadImage("image.bmp");
	txTransparentBlt(0, 0, source_image);
	txDeleteDC(source_image);
}


void fill(double x, double y, COLORREF current) {
	if (current == txGetPixel(x, y) &&
		y > window_for_painting.coords0_.y &&
		y < window_for_painting.coords1_.y &&
		x > window_for_painting.coords0_.x &&
		x < window_for_painting.coords1_.x) {
		for (int i = -1; i <= 1; ++i) {
			for (int j = -1; j <= 1; ++j) {
				fill(x + i, y + j, current);
				txSetPixel(x, y, color_for_painting);
			}
		}
	}
	else {
		return;
	}
}

void cover_for_fill() {
	plot coords = { txMouseX(), txMouseY() };
	COLORREF current = txGetPixel(coords.x, coords.y);
	fill(coords.x, coords.y, current);
}

void draw_pixel(double x, double y, COLORREF color_of_point) {
	txBegin();
	RGBQUAD* pixel = &Video_memory[(int)x + ((int)1300 - (int)y) * 700];
	pixel->rgbRed = txExtractColor(color_of_point, TX_RED);
	pixel->rgbGreen = txExtractColor(color_of_point, TX_GREEN);
	pixel->rgbBlue = txExtractColor(color_of_point, TX_BLUE);
	txSleep();
}