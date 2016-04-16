#ifndef KBD_H
#define KBD_H

class Kbd
{
public:

	Kbd(){}
	~Kbd(){}

	bool right()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) return true; else return false; }
	bool left()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) return true; else return false; }
	bool up()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) return true; else return false; }
	bool down()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) return true; else return false; }
	bool ret()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) return true; else return false; }
	bool escape()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) return true; else return false; }
	bool f1()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::F1)) return true; else return false; }
	bool f2()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::F2)) return true; else return false; }
	bool f3()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::F3)) return true; else return false; }
	bool f4()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::F4)) return true; else return false; }
	bool f5()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::F5)) return true; else return false; }
	bool f6()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::F6)) return true; else return false; }
	bool f7()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::F7)) return true; else return false; }
	bool f8()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::F8)) return true; else return false; }
	bool f9()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::F9)) return true; else return false; }
	bool f10()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::F10)) return true; else return false; }
	bool f11()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::F11)) return true; else return false; }
	bool f12()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::F12)) return true; else return false; }
	bool home()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::Home)) return true; else return false; }
	bool end()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::End)) return true; else return false; }
	bool ctrl()
		{ if ( sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl) ) return true; else return false; }
	bool ctrlA()
		{ if( sf::Keyboard::isKeyPressed(sf::Keyboard::A)
			&& ( sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl) ) )
			return true; else return false; }
	bool ctrlC()
		{ if( sf::Keyboard::isKeyPressed(sf::Keyboard::C)
			&& ( sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl) ) )
			return true; else return false; }
	bool ctrlV()
		{ if( sf::Keyboard::isKeyPressed(sf::Keyboard::V)
			&& ( sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl) ) )
			return true; else return false; }
	bool ctrlZ()
		{ if( sf::Keyboard::isKeyPressed(sf::Keyboard::Z)
			&& ( sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl) ) )
			return true; else return false; }
	bool ctrlS()
		{ if( sf::Keyboard::isKeyPressed(sf::Keyboard::S)
			&& ( sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl) ) )
			return true; else return false; }
	bool ctrlL()
		{ if( sf::Keyboard::isKeyPressed(sf::Keyboard::L)
			&& ( sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl) ) )
			return true; else return false; }
	bool ctrlUp()
		{ if( sf::Keyboard::isKeyPressed(sf::Keyboard::Up)
			&& ( sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl) ) )
			return true; else return false; }
	bool ctrlDown()
		{ if( sf::Keyboard::isKeyPressed(sf::Keyboard::Down)
			&& ( sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl) ) )
			return true; else return false; }
	bool altD()
		{ if( (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt))
			&& sf::Keyboard::isKeyPressed(sf::Keyboard::D) )
			return true; else return false; }
	bool altO()
		{ if( (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt))
			&& sf::Keyboard::isKeyPressed(sf::Keyboard::O) )
			return true; else return false; }
	bool altI()
		{ if( (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt))
			&& sf::Keyboard::isKeyPressed(sf::Keyboard::I) )
			return true; else return false; }
	bool altS()
		{ if( (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt))
			&& sf::Keyboard::isKeyPressed(sf::Keyboard::S) )
			return true; else return false; }
	bool altV()
		{ if( (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt))
			&& sf::Keyboard::isKeyPressed(sf::Keyboard::V) )
			return true; else return false; }
	bool shift()
		{ if( sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) )
			return true; else return false; }
	bool alt()
		{ if( sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt) )
			return true; else return false; }
	bool y()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::Y)) return true; else return false; }
	bool n()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::N)) return true; else return false; }

private:

};

#endif