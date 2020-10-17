#include "Parser.hpp"

Component::~Component() {}

Group::Group(Context& c) : _c(c) {}

Group::~Group() {
		std::vector<Component*>::iterator it = this->_components.begin();
		std::vector<Component*>::iterator ite = this->_components.end();
		while (it != ite) {
			delete (*it);
			it++;
		}
}

Group* Group::add(Component* component) {
	this->_components.push_back(component);
	return this;
}

/* 이게 맞나? 아닌듯 */
void Group::run() {
		/* group() */
    if (!this->_c.state)
			return;
		this->_c.level++;
		this->_c.save.push(this->_c.idx);
	
		std::vector<Component*>::iterator it = this->_components.begin();
		std::vector<Component*>::iterator ite = this->_components.end();
		while (it != ite) {
			(*it)->run();
			it++;
		}

		/* ungroup() */
		if (!this->_c.state)
			return ;
    if (this->_c.skip == this->_c.level) {
        this->_c.skip = INT_MAX;
    }
		this->_c.save.pop();
    this->_c.level--;
}

Accept::Accept(Context& c, bool (*f)(Context& c)) : _c(c) {
	this->_f = f;
}

void Accept::run() {
	if (this->_c.skip <= this->_c.level || !this->_c.state) {
      return;
  } else if (this->_c.idx >= this->_c.str.size()) {
		this->_c.state = false;
		return;
	}
  this->_c.state = this->_f(_c);
}

Accept::~Accept() {}

Repeat::Repeat(Context& c, int min, int max, Component* component) : _c(c) {
	this->_min = min;
	this->_max = max;
	this->_component = component;
}

Repeat::~Repeat() {
	delete _component;
}

void Repeat::run() {
	if (this->_c.skip <= this->_c.level || !this->_c.state) {
        return;
  }
	int i;
	for (i = 0; this->_max == 0 ? true : i < this->_max; i++) {
		this->_c.save.push(this->_c.idx);
		this->_component->run();
		if (this->_c.state == false) {
			this->_c.idx = this->_c.save.top(); this->_c.save.pop();
			break;
		}
		this->_c.save.pop();
	}
	this->_c.state = ((i >= this->_min) && (this->_max == 0 ? true :  i <= this->_max));
}

Alternate::Alternate(Context& c) : _c(c) {}

Alternate::~Alternate() {}

void Alternate::run() {
	if (this->_c.skip <= this->_c.level) {
        return;
    }
    if (this->_c.state) {
        this->_c.skip = this->_c.level;
    } else {
        this->_c.idx = this->_c.save.top();
        this->_c.state = true;
    }
}