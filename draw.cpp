#define _CRT_SECURE_NO_WARNINGS

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
int is_parametrs_changed = 1;
double radius_for_painting = 24;
func_t func_of_drawing = &pencil;
plot coords0_of_but = { 0,0 };
plot coords1_of_but = { 1500, 100 };
plot coords0_of_main_window = { 0, 100 };
plot coords1_of_main_window = { 1500, 900 };

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

class Color_window : public Window {
public:
	virtual void draw_window() override {
		txSetColor(TX_LIGHTGRAY);
		txSetFillColor(TX_WHITE);
		txRectangle(coords0_.x, coords0_.y, coords1_.x, coords1_.y);
		txSetFillColor(color_for_painting);
		txCircle((coords0_.x + coords1_.x)/2, (coords0_.y + coords1_.y) / 2, radius_for_painting / 2);
	}

	Color_window(plot coords0, plot coords1) :
		Window(coords0, coords1)
	{}
};

class BasicButton: public Window {
public:
	virtual void draw_button(){
		txSetColor(TX_LIGHTGRAY);
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
		txSetColor(TX_LIGHTGRAY);
		txSetFillColor(color_);
		txRectangle(coords0_.x, coords0_.y, coords1_.x, coords1_.y);
	}
	virtual void action() override {
		is_parametrs_changed = 1;
		color_for_painting = color_;
	}
	SetColorButton(plot coords0, plot coords1, COLORREF color):
		BasicButton(coords0, coords1),
		color_(color)
	{}
};

class Scroll_bar : public BasicButton {
public:
	COLORREF rgb_color_;
	plot coords_of_bar0_;
	plot coords_of_bar1_;
	double* revolvable_;
	double difference_;
	virtual void draw_button() override {
		txSetColor(TX_LIGHTGRAY, 1);
		txSetFillColor(TX_WHITE);
		txRectangle(coords0_.x - 7, coords0_.y, coords1_.x + 7, coords1_.y);
		coords_of_bar0_ = { (coords1_.x + coords0_.x) / 2 - 7, coords0_.y };
		coords_of_bar1_ = { (coords1_.x + coords0_.x) / 2 + 7, coords1_.y };
		txSetColor(TX_LIGHTGRAY);
		txSetFillColor(rgb_color_);
		txRectangle(coords_of_bar0_.x, coords_of_bar0_.y, coords_of_bar1_.x, coords_of_bar1_.y);
	}
	virtual void action_with_revolvable() {
		*revolvable_ = ((coords_of_bar1_.x + coords_of_bar0_.x) / 2 - coords0_.x) / difference_ + 2;
	}
	
	virtual void action() override {
		is_parametrs_changed = 1;
		double scroll = txMouseX();
		txSetColor(TX_LIGHTGRAY, 1);
		txSetFillColor(TX_WHITE);
		txRectangle(coords0_.x - 7, coords0_.y, coords1_.x + 7, coords1_.y);
		if (scroll > coords0_.x - 7 && scroll < coords1_.x + 7) {
			coords_of_bar0_.x = scroll - 7;
			coords_of_bar1_.x = scroll + 7;
			action_with_revolvable();
			txSetFillColor(rgb_color_);
			txRectangle(coords_of_bar0_.x, coords_of_bar0_.y, coords_of_bar1_.x, coords_of_bar1_.y);
			txSleep(0.1);
		}
	}
	Scroll_bar(plot coords0, plot coords1, double* revolvable, double difference, COLORREF rgb_color = TX_LIGHTGRAY):
		BasicButton(coords0, coords1),
		revolvable_(revolvable),
		difference_(difference),
		rgb_color_(rgb_color)
	{}
};


class RGB_scroll_bar : public Scroll_bar {
public:
	COLORREF* revolvable_color_;
	double revolvable_;
	virtual void action_with_revolvable() override {
		revolvable_ = 255 * ((coords_of_bar1_.x + coords_of_bar0_.x) / 2 - coords0_.x) / (coords1_.x - coords0_.x);
		if (rgb_color_ == TX_LIGHTRED) {
			*revolvable_color_ = RGB(revolvable_, txExtractColor(*revolvable_color_, TX_GREEN), txExtractColor(*revolvable_color_, TX_BLUE));
		} 
		else if (rgb_color_ == TX_LIGHTGREEN) {
			*revolvable_color_ = RGB(txExtractColor(*revolvable_color_, TX_RED), revolvable_, txExtractColor(*revolvable_color_, TX_BLUE));
		}
		else if (rgb_color_ == TX_LIGHTBLUE) {
			*revolvable_color_ = RGB(txExtractColor(*revolvable_color_, TX_RED), txExtractColor(*revolvable_color_, TX_GREEN), revolvable_);
		}
	}

	RGB_scroll_bar(plot coords0, plot coords1, COLORREF* revolvable, COLORREF rgb_color) :
		Scroll_bar(coords0, coords1, NULL, NULL, rgb_color),
		revolvable_color_(revolvable)
	{}
};


class FunctionalButton : public BasicButton {
public:
	const char* name_;
	func_t function_;
	virtual void draw_button() override {
		txSetColor(TX_LIGHTGRAY);
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
	const char* source_image_;
	func_t function_;
	virtual void draw_button() override {
		txSetColor(TX_LIGHTGRAY, 3);
		txSetFillColor(TX_TRANSPARENT);
		HDC source_image = txLoadImage(source_image_);
		txTransparentBlt(coords0_.x, coords0_.y, source_image);
		txDeleteDC(source_image);
		txRectangle(coords0_.x, coords0_.y, coords1_.x, coords1_.y);
	}
	void redraw_button1() {
		txSetColor(TX_GRAY, 5);
		txSetFillColor(TX_TRANSPARENT);
		txRectangle(coords0_.x, coords0_.y, coords1_.x, coords1_.y);
	}
	void redraw_button2() {
		txSetColor(TX_WHITE, 5);
		txSetFillColor(TX_TRANSPARENT);
		txSleep();
		txRectangle(coords0_.x, coords0_.y, coords1_.x, coords1_.y);
		txSetColor(TX_LIGHTGRAY, 3);
		txRectangle(coords0_.x, coords0_.y, coords1_.x, coords1_.y);
	}
	virtual void action() override {
		redraw_button1();
		func_of_drawing = function_;
		redraw_button2();
	}
	InstrumentalButton(plot coords0, plot coords1, func_t function, const char* source_image):
		BasicButton(coords0, coords1),
		function_(function),
		source_image_(source_image)
	{}

};

HDC virtual_canvas = txCreateCompatibleDC(coords1_of_main_window.x - coords0_of_main_window.x, coords1_of_main_window.y - coords0_of_main_window.y);
const int number_of_canvas = 5000;


class Canvas : public BasicButton {
public:
	HDC canvas_[number_of_canvas] = {};
	func_t function_;
	int current_canvos_;

	void add(HDC canvas) {
		int i;
		for (i = 1; canvas_[i] != NULL; ++i) {}
		cout << i;
		canvas_[i] = canvas;
	}

	virtual void draw_button() override {
		txSetColor(TX_LIGHTGRAY);
		txSetFillColor(TX_WHITE, virtual_canvas);
		txRectangle(coords0_.x, coords0_.y - coords0_of_main_window.y, coords1_.x, coords1_.y - coords0_of_main_window.y, virtual_canvas);
		txBitBlt(coords0_of_main_window.x, coords0_of_main_window.y, virtual_canvas);
	}

	virtual void action() override {
		int i;
		//for (i = current_canvos_ + 1; canvas_[i] != NULL; ++i) {
			//canvas_[i] = NULL;
		//}
		function_();
		virtual_canvas = txCreateCompatibleDC(coords1_of_main_window.x - coords0_of_main_window.x, coords1_of_main_window.y - coords0_of_main_window.y);
		txTransparentBlt(virtual_canvas, 0, 0, coords1_of_main_window.x - coords0_of_main_window.x,
						 coords1_of_main_window.y - coords0_of_main_window.y, return_last_canvas());
		add(virtual_canvas);
	}

	HDC return_last_canvas() {
		int i;
		for (i = 0; canvas_[i] != NULL; ++i) {}
		current_canvos_ = i - 1;
		cout << current_canvos_ << "current\n";
			return canvas_[i - 1];
	}

	HDC return_current_canvos() {
		if (current_canvos_ <= 0) {
			//cout << "000" << "\n";
			current_canvos_ = 0;
			return canvas_[0];
		}
		else if (canvas_[current_canvos_] == NULL) {
			//cout << "NULL" << "\n";
			return canvas_[0];
		}
		else {
			//cout << "NORM" << "\n";
			return canvas_[current_canvos_];
		}
	}

	Canvas(plot coords0, plot coords1, func_t function, int current_canvos_ = 1):
		BasicButton(coords0, coords1),
		function_(function)
	{
		canvas_[0] = virtual_canvas;
	}

	~Canvas() {
		for (int i = 0; canvas_[i] != NULL; ++i) {
			txDeleteDC(canvas_[i]);
			txDeleteDC(virtual_canvas);
		}
	}
};


const int number_of_buttons = 50;
Canvas window_for_painting({ 0, 100 }, { 1500, 900 }, func_of_drawing);
Window window_for_buttons({ 0, 0 }, { 1500, 100 });
Color_window indicator({ 670, 0 }, { 760, 90 });

class ManagerButton {
public:
	BasicButton* buttons_[number_of_buttons];
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
			if (is_parametrs_changed == 1) {
				indicator.draw_window();
				is_parametrs_changed = 0;
			}

		}
	}

	ManagerButton() {
		for (int i = 0; i < number_of_buttons; i++) {
			buttons_[i] = NULL;
		}
	}
	~ManagerButton() {
		for (int i = 0; buttons_[i] != NULL; ++i) {
			delete buttons_[i];
		}
	}
};


void clear();
void exit_from_program();
void save_image();
void open_image();
void ctlr_z();
void ctlr_y();

ManagerButton manager;

int main() {
	txCreateWindow(1500, 900);
	window_for_buttons.draw_window();

	manager.add(new SetColorButton(  { 0, 0 },   { 25, 25 }, TX_LIGHTBLUE));
	manager.add(new SetColorButton(  { 25, 0 },  { 50, 25 }, TX_LIGHTRED));
	manager.add(new SetColorButton(  { 50, 0 }, { 75, 25 },  TX_YELLOW));

	manager.add(new FunctionalButton({ 200, 0 }, { 250, 50 }, "clear", clear));
	manager.add(new FunctionalButton({ 250,0 },  { 350, 50 }, "exit", exit_from_program));
	manager.add(new FunctionalButton({ 350,0 }, { 450, 50 }, "save", save_image));
	manager.add(new FunctionalButton({ 450,0 }, { 550, 50 }, "open", open_image));
	manager.add(new FunctionalButton({ 760,0 }, { 810, 50 }, "<<", ctlr_z));
	manager.add(new FunctionalButton({ 810,0 }, { 860, 50 }, ">>", ctlr_y));

	manager.add(new InstrumentalButton({ 150, 6 }, { 180, 36 }, pencil, "pencil.bmp"));
	manager.add(new InstrumentalButton({ 120, 6 }, { 150, 36 }, cover_for_fill, "filling.bmp"));

	manager.add(new Scroll_bar({ 0,50 }, { 250, 100 }, &radius_for_painting, 6));

	manager.add(new RGB_scroll_bar({ 560,0 }, { 660, 30 }, &color_for_painting, TX_LIGHTRED));
	manager.add(new RGB_scroll_bar({ 560,30 }, { 660, 60 }, &color_for_painting, TX_LIGHTGREEN));
	manager.add(new RGB_scroll_bar({ 560,60 }, { 660, 90 }, &color_for_painting, TX_LIGHTBLUE));

	manager.add(&window_for_painting);

	manager.draw_buttons();

	manager.run();
}
void clear() {
	window_for_painting.draw_window();
}

void exit_from_program() {
	window_for_painting.~Canvas();
	exit(0);
}

void draw_point(plot coords, COLORREF color_of_point, double R) {
	txSetFillColor(color_of_point);
	txSetColor(color_of_point);
	txCircle(coords.x, coords.y, R);
}


void pencil() {
	plot coords1 = { txMouseX(), txMouseY() };
	while (txMouseButtons() == 1) {
		plot coords2 = { txMouseX(), txMouseY() };
		txSetColor(color_for_painting, radius_for_painting, window_for_painting.return_last_canvas());
		txLine(coords1.x, coords1.y - coords0_of_main_window.y, coords2.x, coords2.y - coords0_of_main_window.y, window_for_painting.return_last_canvas());
		txBitBlt(coords0_of_main_window.x, coords0_of_main_window.y, window_for_painting.return_last_canvas());
		txSleep();
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
	HDC canvas = window_for_painting.return_last_canvas();
	if (current == txGetPixel(x, y) &&
		y > window_for_painting.coords0_.y &&
		y < window_for_painting.coords1_.y &&
		x > window_for_painting.coords0_.x &&
		x < window_for_painting.coords1_.x) {
		for (int i = -1; i <= 1; ++i) {
			for (int j = -1; j <= 1; ++j) {
				fill(x + i, y + j, current);
				txSetPixel(x, y - coords0_of_main_window.y, color_for_painting, canvas);
				txSetPixel(x, y, color_for_painting);
			}
		}
	}
	else {
		return;
	}
	//FILE* file = fopen("actions.txt", "w");
	//fprintf(file, "filling was activated %f %f %d\n ", x, y, current);
	//fclose(file);
}

void cover_for_fill() {
	plot coords = { txMouseX(), txMouseY() };
	COLORREF current = txGetPixel(coords.x, coords.y);
	fill(coords.x, coords.y, current);
}

void ctlr_z() {
	window_for_painting.current_canvos_ -= 1;
	cout << window_for_painting.current_canvos_ << "zz\n";
	txBitBlt(coords0_of_main_window.x, coords0_of_main_window.y, window_for_painting.canvas_[0]);
}

void ctlr_y() {
	window_for_painting.current_canvos_ += 1;
	cout << window_for_painting.current_canvos_ << "yy\n";
	txBitBlt(coords0_of_main_window.x, coords0_of_main_window.y, window_for_painting.return_current_canvos());
}