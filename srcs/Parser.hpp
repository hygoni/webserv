#ifndef SRCS_PARSER_HPP_
#define SRCS_PARSER_HPP_

/* Copyright 2020 hyeyoo, hyekim */

#include <string>
#include <stack>
#include <vector>
#include <cstdint>
#include "Context.hpp"
/*
* 일련의 Component들을 저장하는 클래스
*/
// class Begin {
// 	private:
// 		vector<Component> components;
// 	public:
// 		Begin& add(Component const& component);
// 		void run();
// };

// Begin& Begin::add(Component const& component) {
// 	this->_components(component);
// }

// void Begin::run() {
// 	/* begin() */
//   this->_c.level++;
//   this->_c.save.push(c.idx);

// 	std::vector<Component>::iterator it = this->_components.begin();
// 	std::vector<Component>::iterator ite = this->_components.end();
// 	while (it != ite) {
// 			(*it).run();
// 			it++;
// 	}

// 	/* end() */
//   this->_c.level--;
// 	if ((unsigned long)(this->_c.idx) != this->_c.str.size())
// 		this->_c.state = false;
// 	this->_c.save.pop();
// }

/*
* group, ungroup, accept, repeat, alter 등에 대한 인터페이스
* run으로 해당 Component를 실행하는 것을 정의할 수 있음.
*/
class Group;

class Component {
  protected:
    Context& _c;
	public:
    Component(Context& c);
		virtual Group operator+(Component const& component);
    virtual ~Component();
    virtual void run() const = 0;
};

/*
사용법 :
Group().add(new Accept<char>(c, 'ch', f))
.add(new Accept<char>(c, std::string("hello"), f))
.run();
*/
class Group : public Component {
	private:
		std::vector<const Component*> _components;
	public:
		Group(Context& c);
		virtual ~Group();
    virtual Group operator+(Component const& component);
		Group add(const Component& component);
    virtual void run() const;
};

class Accept : public Component {
	private:
		bool (*_f)(Context& c);
	public:
		virtual ~Accept();
		Accept(Context &c, bool (*f)(Context& c));
		virtual void run() const;
};

template <typename T>
class AcceptTemplate : public Component {
	private:
		T _arg;
		bool (*_f)(Context& c, T arg);
	public:
		virtual ~AcceptTemplate();
		AcceptTemplate(Context &c, T arg, bool (*f)(Context& c, T arg));
		virtual void run() const;
};

template <typename T>
AcceptTemplate<T>::AcceptTemplate(Context &c, T arg, bool (*f)(Context& c, T arg)) : Component(c) {
	this->_arg = arg;
	this->_f = f;
}

template <typename T>
AcceptTemplate<T>::~AcceptTemplate() {}

template <typename T>
void AcceptTemplate<T>::run() const {
  if (this->_c.skip <= this->_c.level || !this->_c.state) {
      return;
  } else if (this->_c.idx >= this->_c.str.size()) {
		this->_c.state = false;
		return;
	}
  this->_c.state = this->_f(_c, _arg);
}

class Repeat : public Component {
	private:
		const Component* _component;
		int _min;
		int _max;
	public:
		Repeat(Context& c, int min, int max, Component* component);
		virtual ~Repeat();
		virtual void run() const;
};

class Alternate : public Component {
	public:
		virtual ~Alternate();
		Alternate(Context& c);
		virtual void run() const;
};

#endif  // SRCS_PARSER_HPP_
