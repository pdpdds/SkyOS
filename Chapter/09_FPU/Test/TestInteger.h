#pragma once
class TestInteger
{
public: constexpr TestInteger() { }
		constexpr TestInteger(int value) :m_Value(value) { }
		constexpr operator int() const { return m_Value; }  
private: int m_Value = 2; 
};