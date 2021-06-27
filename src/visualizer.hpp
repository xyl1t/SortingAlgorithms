#ifndef VISUALIZER_HPP
#define VISUALIZER_HPP

class Visualizer {
private:
	float speed;

public:
	virtual void highlight(int index) = 0;
};

#endif