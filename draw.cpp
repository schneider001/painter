#define _CRT_SECURE_NO_WARNINGS

#include "D:\Txlib\TX\TXlib.h"
#include <stdlib.h>
#include <string>
#include <cmath>
#include <ctime>
#pragma comment(linker, "/STACK:257772160")

RGBQUAD * Video_memory = NULL;
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
		txSetColor(TX_LIGHTGRAY);
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
		txCircle((coords0_.x + coords1_.x) / 2, (coords0_.y + coords1_.y) / 2, radius_for_painting / 2);
	}

	Color_window(plot coords0, plot coords1) :
		Window(coords0, coords1)
	{}
};


class BasicButton : public Window {
public:
	virtual void draw_button() {
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

	BasicButton(plot coords0, plot coords1) :
		Window(coords0, coords1)
	{}
};


class SetColorButton : public BasicButton {
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
	SetColorButton(plot coords0, plot coords1, COLORREF color) :
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
	Scroll_bar(plot coords0, plot coords1, double* revolvable, double difference, COLORREF rgb_color = TX_LIGHTGRAY) :
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
		if (rgb_color_ == RGB(255, 36, 0)) {
			*revolvable_color_ = RGB(revolvable_, txExtractColor(*revolvable_color_, TX_GREEN), txExtractColor(*revolvable_color_, TX_BLUE));
		}
		else if (rgb_color_ == RGB(102, 255, 0)) {
			*revolvable_color_ = RGB(txExtractColor(*revolvable_color_, TX_RED), revolvable_, txExtractColor(*revolvable_color_, TX_BLUE));
		}
		else if (rgb_color_ == RGB(125, 249, 255)) {
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
	FunctionalButton(plot coords0, plot coords1, const char* name, func_t function) :
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
	InstrumentalButton(plot coords0, plot coords1, func_t function, const char* source_image) :
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
	int index_of_canvas_;

	void add(HDC canvas) {
		int i;
		for (i = 0; canvas_[i] != NULL; ++i) {}
		index_of_canvas_ = i - 1;
		canvas_[i] = txCreateCompatibleDC(coords1_of_main_window.x - coords0_of_main_window.x, coords1_of_main_window.y - coords0_of_main_window.y);
		txBitBlt(canvas_[i], 0, 0, coords1_of_main_window.x - coords0_of_main_window.x,
				 coords1_of_main_window.y - coords0_of_main_window.y, canvas);
	}

	virtual void draw_button() override {
		txSetColor(TX_LIGHTGRAY, 2, virtual_canvas);
		txSetFillColor(TX_WHITE, virtual_canvas);
		txRectangle(coords0_.x, coords0_.y - coords0_of_main_window.y, coords1_.x, coords1_.y - coords0_of_main_window.y, virtual_canvas);
		txBitBlt(coords0_of_main_window.x, coords0_of_main_window.y, virtual_canvas);
		add(virtual_canvas);
		add(virtual_canvas);
	}

	virtual void action() override {
		virtual_canvas = txCreateCompatibleDC(coords1_of_main_window.x - coords0_of_main_window.x, coords1_of_main_window.y - coords0_of_main_window.y);
		int i;
		for (i = index_of_canvas_ + 2; canvas_[i] != NULL; ++i) {
			canvas_[i] = NULL;
		}
		txBitBlt(canvas_[index_of_canvas_ + 1], 0, 0, coords1_of_main_window.x - coords0_of_main_window.x,
			coords1_of_main_window.y - coords0_of_main_window.y, canvas_[index_of_canvas_]);
		function_();
		txBitBlt(virtual_canvas, 0, 0, coords1_of_main_window.x - coords0_of_main_window.x,
						 coords1_of_main_window.y - coords0_of_main_window.y, return_last_canvas());
		add(virtual_canvas);
	}

	HDC return_last_canvas() {
		int i;
		for (i = 0; canvas_[i] != NULL; ++i) {}
		index_of_canvas_ = i - 1;
		return canvas_[i - 1];
	}

	HDC return_current_canvos() {
		if (index_of_canvas_ <= 0) {
			index_of_canvas_ = 0;
			return canvas_[0];
		}
		else if (canvas_[index_of_canvas_] == NULL) {
			index_of_canvas_ = index_of_canvas_ - 1;
			return return_last_canvas();
		}
		else {
			return canvas_[index_of_canvas_];
		}
	}

	Canvas(plot coords0, plot coords1, func_t function, int index_of_canvas_ = 0) :
		BasicButton(coords0, coords1),
		function_(function)
	{}

	~Canvas() {
		for (int i = 0; canvas_[i] != NULL; ++i) {
			txDeleteDC(canvas_[i]);
		}
			txDeleteDC(virtual_canvas);
	}
};


const int number_of_buttons = 50;
Canvas window_for_painting({ 0, 100 }, { 1500, 900 }, func_of_drawing);
Window window_for_buttons({ 0, 0 }, { 1500, 100 });
Color_window indicator({ 910, 3 }, { 1003, 96 });


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
void spray();


ManagerButton manager;


int main() {
	txCreateWindow(1500, 900);

	manager.add(&window_for_painting);

	manager.add(new SetColorButton({ 555, 10 }, { 580, 35 }, TX_LIGHTBLUE));
	manager.add(new SetColorButton({ 580, 10 }, { 605, 35 }, TX_LIGHTRED ));
	manager.add(new SetColorButton({ 605, 10 }, { 630, 35 }, TX_LIGHTGREEN));
	manager.add(new SetColorButton({ 630, 10 }, { 655, 35 }, TX_YELLOW));
	manager.add(new SetColorButton({ 655, 10 }, { 680, 35 }, TX_ORANGE));
	manager.add(new SetColorButton({ 680, 10 }, { 705, 35 }, TX_PINK));
	manager.add(new SetColorButton({ 705, 10 }, { 730, 35 }, TX_MAGENTA));

	manager.add(new FunctionalButton({   0,  0 }, { 100,  50 }, "open",  open_image));
	manager.add(new FunctionalButton({ 100,  0 }, { 200,  50 }, "save",  save_image));
	manager.add(new FunctionalButton({ 100, 50 }, { 200, 100 }, "exit",  exit_from_program));
	manager.add(new FunctionalButton({ 1020,20 }, { 1120,80 }, "clear", clear ));
	manager.add(new FunctionalButton({   0, 50 }, { 50,  100 }, "<<",    ctlr_z));
	manager.add(new FunctionalButton({	50, 50 }, { 100, 100 }, ">>",    ctlr_y));

	manager.add(new InstrumentalButton({ 260, 10 }, { 290, 40 }, pencil,         "pencil.bmp" ));
	manager.add(new InstrumentalButton({ 290, 10 }, { 320, 40 }, spray,          "spray.bmp"  ));
	manager.add(new InstrumentalButton({ 320, 10 }, { 350, 40 }, cover_for_fill, "filling.bmp"));

	manager.add(new Scroll_bar({ 267, 60 }, { 507, 90 }, &radius_for_painting, 6));

	manager.add(new RGB_scroll_bar({ 790,  3 }, { 900, 32 }, &color_for_painting, RGB(255, 36,  0)));
	manager.add(new RGB_scroll_bar({ 790, 35 }, { 900, 64 }, &color_for_painting, RGB(102, 255, 0)));
	manager.add(new RGB_scroll_bar({ 790, 67 }, { 900, 96 }, &color_for_painting, RGB(125, 249, 255)));

	window_for_buttons.draw_window();

	manager.draw_buttons();

	manager.run();
}


void clear() {
	txBitBlt(window_for_painting.canvas_[window_for_painting.index_of_canvas_], 0, 0, coords1_of_main_window.x - coords0_of_main_window.x,
		coords1_of_main_window.y - coords0_of_main_window.y, window_for_painting.canvas_[0]);
	txBitBlt(coords0_of_main_window.x, coords0_of_main_window.y, window_for_painting.canvas_[0]);
}


void exit_from_program() {
	window_for_painting.~Canvas();
	exit(0);
}


/*void pencil() {
	plot coords1 = { txMouseX(), txMouseY() };
	while (txMouseButtons() == 1) {
		plot coords2 = { txMouseX(), txMouseY() };
		txSetColor(color_for_painting, radius_for_painting);
		txLine(coords1.x, coords1.y, coords2.x, coords2.y);
		coords1 = coords2;
	}
}


void spray() {
	srand(time(0));
	int random = rand();
	while (txMouseButtons() == 1) {
		plot coords = { txMouseX(), txMouseY() };
		for (int x = -radius_for_painting / 2; x <= radius_for_painting / 2; x++) {
			for (int y = -radius_for_painting / 2; y <= radius_for_painting / 2; y++) {
				if (sqrt(x * x + y * y) <= radius_for_painting / 2) {
					srand(random);
					random = rand();
					if (random % 2 == 1) {
						txSetPixel(coords.x - x, coords.y - y, color_for_painting);
					}
					int time = 0;
					while (time < 5000)
						time++;
				}
				if (txMouseButtons() == 0)
					break;
			}
		}
		txSleep(1);
	}
}*/


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


void spray() {
	srand(time(0));
	int random = rand();
	while (txMouseButtons() == 1) {
		plot coords = { txMouseX(), txMouseY() };
		for (int x = -radius_for_painting / 2; x <= radius_for_painting / 2; x++) {
			for (int y = -radius_for_painting / 2; y <= radius_for_painting / 2; y++) {
				if ((sqrt(x * x + y * y) <= radius_for_painting / 2) && (coords.y - y > coords0_of_main_window.y)) {
					srand(random);
					random = rand();
					if (random % 2 == 1) {
						txSetPixel(coords.x - x, coords.y - y, color_for_painting);
						txSetPixel(coords.x - x, coords.y - y - coords0_of_main_window.y, color_for_painting, window_for_painting.return_last_canvas());
					}
					int time = 0;
					while (time < 7000)
						time++;
				}
				if (txMouseButtons() == 0)
					break;
			}
		}
		txSleep(1);
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
}


/*void fill(double x, double y, COLORREF current) {
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
}*/


void cover_for_fill() {
	plot coords = { txMouseX(), txMouseY() };
	COLORREF current = txGetPixel(coords.x, coords.y);
	fill(coords.x, coords.y, current);
}


void ctlr_z() {
	if (window_for_painting.canvas_[window_for_painting.index_of_canvas_ + 1] == NULL)
		window_for_painting.index_of_canvas_ -= 2;
	else
		window_for_painting.index_of_canvas_ -= 1;
	txBitBlt(coords0_of_main_window.x, coords0_of_main_window.y, window_for_painting.return_current_canvos());
}


void ctlr_y() {
	if (window_for_painting.canvas_[window_for_painting.index_of_canvas_ + 1] != NULL)
		window_for_painting.index_of_canvas_ += 1;
	txBitBlt(coords0_of_main_window.x, coords0_of_main_window.y, window_for_painting.return_current_canvos());
	if (window_for_painting.canvas_[window_for_painting.index_of_canvas_ + 1] == NULL)
		window_for_painting.index_of_canvas_ -= 1;
}
