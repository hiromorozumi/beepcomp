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
	bool pageUp()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::PageUp)) return true; else return false; }		
	bool pageDown()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::PageDown)) return true; else return false; }
	bool ctrl()
		{ if ( sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl) ) return true; else return false; }
	bool leftCtrl()
		{ if ( sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ) return true; else return false; }
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
	bool ctrlG()
		{ if( sf::Keyboard::isKeyPressed(sf::Keyboard::G)
			&& ( sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl) ) )
			return true; else return false; }
	bool ctrlR()
		{ if( sf::Keyboard::isKeyPressed(sf::Keyboard::R)
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
	bool altX()
		{ if( (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt))
			&& sf::Keyboard::isKeyPressed(sf::Keyboard::X) )
			return true; else return false; }
	bool altZ()
		{ if( (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt))
			&& sf::Keyboard::isKeyPressed(sf::Keyboard::Z) )
			return true; else return false; }
	bool shift()
		{ if( sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) )
			return true; else return false; }
	bool alt()
		{ if( sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt) )
			return true; else return false; }
	bool leftAlt()
		{ if( sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) )
			return true; else return false; }

	bool a()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) return true; else return false; }
	bool b()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::B)) return true; else return false; }
	bool c()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::C)) return true; else return false; }
	bool d()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) return true; else return false; }
	bool e()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::E)) return true; else return false; }
	bool f()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::F)) return true; else return false; }
	bool g()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::G)) return true; else return false; }
	bool h()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::H)) return true; else return false; }
	bool i()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::I)) return true; else return false; }
	bool j()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::J)) return true; else return false; }
	bool k()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::K)) return true; else return false; }
	bool l()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::L)) return true; else return false; }
	bool m()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::M)) return true; else return false; }
	bool n()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::N)) return true; else return false; }
	bool o()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::O)) return true; else return false; }
	bool p()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::P)) return true; else return false; }
	bool q()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) return true; else return false; }
	bool r()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::R)) return true; else return false; }
	bool s()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) return true; else return false; }
	bool t()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::T)) return true; else return false; }
	bool u()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::U)) return true; else return false; }
	bool v()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::V)) return true; else return false; }
	bool w()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) return true; else return false; }
	bool x()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::X)) return true; else return false; }
	bool y()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::Y)) return true; else return false; }
	bool z()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) return true; else return false; }
		
	bool letterKey(std::string key)
	{
		if( key=="A" && a() ) return true;
		if( key=="B" && b() ) return true;
		if( key=="C" && c() ) return true;
		if( key=="D" && d() ) return true;
		if( key=="E" && e() ) return true;
		if( key=="F" && f() ) return true;
		if( key=="G" && g() ) return true;
		if( key=="H" && h() ) return true;
		if( key=="I" && i() ) return true;
		if( key=="J" && j() ) return true;
		if( key=="K" && k() ) return true;
		if( key=="L" && l() ) return true;
		if( key=="M" && m() ) return true;
		if( key=="N" && n() ) return true;
		if( key=="O" && o() ) return true;
		if( key=="P" && p() ) return true;
		if( key=="Q" && q() ) return true;
		if( key=="R" && r() ) return true;
		if( key=="S" && s() ) return true;
		if( key=="T" && t() ) return true;
		if( key=="U" && u() ) return true;
		if( key=="V" && v() ) return true;
		if( key=="W" && w() ) return true;
		if( key=="X" && x() ) return true;	
		if( key=="Y" && y() ) return true;
		if( key=="Z" && z() ) return true;			
		return false;
	}
private:

};

#endif