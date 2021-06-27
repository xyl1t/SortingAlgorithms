#define CIDR_IMPLEMENTATION
#include "cidr.hpp"
#include "eventHandler.hpp"
#include "display.hpp"
#include "beeper.hpp"
#include "common.hpp"
#include "timer.hpp"
using namespace cdr;

#include <iostream>
#include <array>
#include <vector>
#include <ctime>
#include <algorithm>
#include <functional>
#include <thread>
#include <atomic>
#include <string>
#include <SDL.h>

std::mutex mtx;

void goThroughData(std::vector<float>& data, std::function<void(int)> highlight) {
	for (int i = 0; i < data.size(); i++) {
		std::this_thread::sleep_for(std::chrono::milliseconds(3));
		highlight(i);
	}
}
void shuffle(std::vector<float>& data, std::function<void(int)> highlight) {
	swaps = 0;
	comparisons = 0;
	constexpr int shuffleFactor = 9;
	for (int i = 0; i < data.size()*shuffleFactor; i++) {
		std::this_thread::sleep_for(std::chrono::milliseconds(speed));
		mtx.lock();
		std::swap(data[i/shuffleFactor], data[rand()%data.size()]);
		mtx.unlock();
		swaps++;
		highlight(i/shuffleFactor);
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	highlight(-1);
}
void bubbleSort(std::vector<float>& data, std::function<void(int)> highlight) {
	swaps = 0;
	comparisons = 0;
	for (int i = 0; i < data.size(); i++) {
		for (int j = 1; j < data.size() - i; j++) {
			std::this_thread::sleep_for(std::chrono::milliseconds(speed));
			
			highlight(j);
			
			comparisons++;
			if (data[j - 1] > data[j]) {
				mtx.lock();
				std::swap(data[j - 1], data[j]);
				mtx.unlock();
				swaps++;
			}
		}
		if (swaps == 0) break;
	}
	if (swaps != 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		goThroughData(data, highlight);
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	highlight(-1);
}
void selectionSort(std::vector<float>& data, std::function<void(int)> highlight) {
	swaps = 0;
	comparisons = 0;
	for (int i = 0; i < data.size() - 1; i++) {
		int min_idx = i;
		for (int j = i + 1; j < data.size(); j++) {
			std::this_thread::sleep_for(std::chrono::milliseconds(speed));
			highlight(j);
			comparisons++;
			if (data[j] < data[min_idx]) {
				min_idx = j;
			}
		}
		if (min_idx != i) {
			mtx.lock();
			std::swap(data[i], data[min_idx]);
			mtx.unlock();
			swaps++;
		}
		if (swaps == 0) {
			break;
		}
	}
	if (swaps != 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		goThroughData(data, highlight);
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	highlight(-1);
}

void heapify(std::vector<float>& data, int n, int i, std::function<void(int)> highlight)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(speed));
	highlight(i);
    int largest = i; // Initialize largest as root
    int l = 2 * i + 1; // left = 2*i + 1
    int r = 2 * i + 2; // right = 2*i + 2
 
    // If left child is larger than root
	comparisons++;
    if (l < n && data[l] > data[largest])
        largest = l;
 
    // If right child is larger than largest so far
	comparisons++;
    if (r < n && data[r] > data[largest])
        largest = r;
 
    // If largest is not root
    if (largest != i) {
		mtx.lock();
        std::swap(data[i], data[largest]);
		mtx.unlock();
		swaps++;
		
        // Recursively heapify the affected sub-tree
        heapify(data, n, largest, highlight);
    }
}
void heapifyOnly(std::vector<float>& data, std::function<void(int)> highlight) {
	swaps = 0;
	comparisons = 0;
	int n = data.size();
    // Build heap (rearrange array)
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(data, n, i, highlight);
		
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	highlight(-1);
}
void heapSort(std::vector<float>& data, std::function<void(int)> highlight)
{
	comparisons = 0;
	swaps = 0;
	int n = data.size();
    // Build heap (rearrange array)
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(data, n, i, highlight);
 
    // One by one extract an element from heap
    for (int i = n - 1; i > 0; i--) {
        // Move current root to end
		std::this_thread::sleep_for(std::chrono::milliseconds(speed));
		highlight(i);
		mtx.lock();
        std::swap(data[0], data[i]);
		mtx.unlock();
		swaps++;
 
        // call max heapify on the reduced heap
        heapify(data, i, 0, highlight);
    }
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	highlight(-1);
}


int main(int argc, char** argv) {
	SDL_Init(SDL_INIT_EVERYTHING);
	srand(time(nullptr));
    
	Timer t;
	Beeper b;
	
	constexpr float zoom = 1.f;
	constexpr int DISPLAY_WIDTH = 1024*zoom;
	constexpr int DISPLAY_HEIGHT = 600*zoom;
	Display display{DISPLAY_WIDTH, DISPLAY_HEIGHT, "Sorting Algorithm Visualisation", true, true, zoom, zoom};
	Renderer r {display.GetPixels(), DISPLAY_WIDTH, DISPLAY_HEIGHT};
	EventHandler& eh = EventHandler::GetInstance();
	float currentTime = 0.f, previousTime = 0.f, delta = 0.f;
	int timer = 0;
	
	bool dataChanged = false;
	std::vector<float> oldData;
	std::vector<float> data;
	std::vector<RGBA> dataColor;
	constexpr int dataSize = 100;
	for (int i = 0; i < dataSize; i++) {
		// float val = static_cast<float>(rand()) / static_cast <float> (RAND_MAX);
		float val = (i+1) / (float)dataSize;
		data.push_back(val);
		oldData.push_back(val);
		dataColor.push_back(RGBA::White);
	}
	
	int width = display.GetCanvasWidth() * 0.9;
	int height = display.GetCanvasHeight() * 0.9;
	int offsetX = display.GetCanvasWidth() - width;
	int offsetY = display.GetCanvasHeight() - height;
	int colHeight = height - offsetY;
	float colWidth = width / (float)data.size();
	
	int oldHighlightIdx = -1;
	int highlightIdx = -1;
	auto highlight = [&](int idx) {
		highlightIdx = idx;
		dataChanged = true;
	};
	
	int old = -1;
	int current = -1;
	std::vector<std::function<void(std::vector<float>&, std::function<void(int)>)>> sortingFunctions = {
		shuffle,
		bubbleSort,
		heapSort,
		heapifyOnly,
		selectionSort,
	};
	std::thread currentThread;
    std::atomic<bool> sortingFinished(true); // Use an atomic flag.
	bool reseted = false;
	
	while (!display.IsClosed()) {
		
		currentTime = SDL_GetTicks(); delta = currentTime - previousTime; previousTime = currentTime;
		if (currentTime / 1000 > timer) { std::clog << "FPS: " << 1000.f / delta  << std::endl; timer++; }
		delta /= 1000;
		
		/* Events *************************************************/
		if (eh.IsKeyPressed(SDL_SCANCODE_SPACE)) { old = -1; current = 0; }
		if (eh.IsKeyPressed(SDL_SCANCODE_B)) { old = -1; current = 1; } 
		if (eh.IsKeyPressed(SDL_SCANCODE_H)) { old = -1; current = 2; }
		if (eh.IsKeyPressed(SDL_SCANCODE_H) && eh.IsKeyDown(SDL_SCANCODE_LSHIFT)) { old = -1; current = 3; }
		if (eh.IsKeyPressed(SDL_SCANCODE_S)) { old = -1; current = 4; }
		
		if (eh.IsKeyDown(SDL_SCANCODE_LEFT)) { 
			data.erase(data.end() - 1);
			oldData.erase(oldData.end() - 1);
			dataColor.erase(dataColor.end() - 1);
			for (int i = 0; i < data.size(); i++) {
				data[i] = oldData[i] = i / (float)data.size();
			}
			colWidth = width / (float)data.size(); 
		}
		if (eh.IsKeyDown(SDL_SCANCODE_RIGHT)) { 
			float val = (data.size()+1) / (float)data.size();
			data.push_back(val);
			oldData.push_back(val);
			dataColor.push_back(RGBA::White);
			for (int i = 0; i < data.size(); i++) {
				data[i] = oldData[i] = i / (float)data.size();
			}
			colWidth = width / (float)data.size(); 
		}
		if (eh.IsKeyDown(SDL_SCANCODE_UP)) { if (speed < 1000) speed++; }
		if (eh.IsKeyDown(SDL_SCANCODE_DOWN)) { if (speed > 0) speed--; }
		
		if (old != current) {
			if (currentThread.joinable())
				currentThread.join();
			old = current;
			sortingFinished = false;
			reseted = false;
			currentThread = std::thread{ [&]() { 
				sortingFunctions[current](data, highlight); 
				sortingFinished = true; 
			}};
			sortTime = -1;
			t.reset();
			currentThread.detach();
		}
		
		/* Logic *************************************************/
		for (int i = 0; i < data.size(); i++) {
			if (oldData[i] != data[i]) {
				dataChanged = true; break;
			} 
		}
		dataChanged |= oldHighlightIdx != highlightIdx;

		if (sortingFinished && !reseted) {
			reseted = true;
			for (int i = 0; i < data.size(); i++) {
				dataColor[i] = RGB::White;
			}
		} else if (dataChanged) {
			dataChanged = false;
			
			for (int i = 0; i < data.size(); i++) {
				dataColor[i] = RGB::White;

				if (oldData[i] != data[i]) {
					oldData[i] = data[i];
					dataColor[i] = RGB::Red;
				}
			}

			oldHighlightIdx = highlightIdx;
			if (highlightIdx > -1) {
				dataColor[highlightIdx] = RGB::Green;
				b.beep(100 + data[highlightIdx] * 5000, 10);
				// b.wait();
				b.beep(100 + data[highlightIdx] * 1000, 3);
			}
		}
		
		
		/* DRAW *************************************************/
		r.Clear(RGB(10, 20, 30));
		for (int i = 0; i < data.size(); i++) {
			int x = i * colWidth + offsetX/2;
			int y = height - std::floor(data[i] * colHeight);
			
			r.FillRectangle(dataColor[i], 
			x, y, 
			std::ceil((i + 1) * colWidth - i * colWidth), data[i] * colHeight);
			// r.DrawRectangle(RGBA::Blue, 
			// r.DrawRectangle(RGBA::Black, 
			// x, y, 
			// colWidth, data[i] * colHeight);
		}
		
		if (sortingFinished) {
			if (sortTime < 0) {
				sortTime = t.elapsed();
			}
			std::string txt_name = "Name: ";
			switch(current) {
				case 0: txt_name += "shuffle"; break;
				case 1: txt_name += "bubble sort"; break;
				case 2: txt_name += "heapsort"; break;
				case 3: txt_name += "heapify"; break;
				case 4: txt_name += "selection sort"; break;
			}
			std::string txt_swaps = "swaps: " + std::to_string(swaps);
			std::string txt_comparisons = "comparisons: " + std::to_string(comparisons);
			std::string txt_time = "time: " + std::to_string(sortTime);
			std::string txt_cols = "cols: " + std::to_string(data.size());
			std::string txt_speed = "sleep: " + std::to_string(speed) + "ms";
			std::string text = txt_name + '\n' + txt_swaps + '\n' + txt_comparisons + '\n' + txt_time + "\n\n" + txt_cols + '\n' + txt_speed;
			r.DrawText(text);
			
			r.DrawText(" [Space]: Shuffle", offsetX / 2 + 0/5.f * width, height + Fonts::Raster8x12.GetFontHeight() * 1, TextAlignment::TL);
			r.DrawText("  [B]ubble sort  ", offsetX / 2 + 1/5.f * width, height + Fonts::Raster8x12.GetFontHeight() * 1, TextAlignment::TL);
			r.DrawText("[S]election sort", offsetX / 2 + 2/5.f * width, height + Fonts::Raster8x12.GetFontHeight() * 1, TextAlignment::TL);
			r.DrawText("   [H]eapsort    ", offsetX / 2 + 3/5.f * width, height + Fonts::Raster8x12.GetFontHeight() * 1, TextAlignment::TL);
			r.DrawText("Shift + [H]eapify", offsetX / 2 + 4/5.f * width, height + Fonts::Raster8x12.GetFontHeight() * 1, TextAlignment::TL);
			
			r.DrawText("  [Up] speed up", offsetX / 2 + 0/5.f * width, height + Fonts::Raster8x12.GetFontHeight() * 3, TextAlignment::TL);
			r.DrawText("[Down] slow down", offsetX / 2 + 1/5.f * width, height + Fonts::Raster8x12.GetFontHeight() * 3, TextAlignment::TL);
			r.DrawText("[Left] Less cols", offsetX / 2 + 2/5.f * width, height + Fonts::Raster8x12.GetFontHeight() * 3, TextAlignment::TL);
			r.DrawText("[Right] More cols", offsetX / 2 + 3/5.f * width, height + Fonts::Raster8x12.GetFontHeight() * 3, TextAlignment::TL);
		}
		
		display.Update();
	}
		
	if (currentThread.joinable())
		currentThread.join();

	
	SDL_Quit();
	
	return 0;
}
