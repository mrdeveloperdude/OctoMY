#include "Context.hpp"



Context::Context()
	: m_line(0)
	, m_column(0)
	, m_token(0)
	, m_valid(false)
{
}

int Context::line()
{
	return m_line;
}

int Context::column()
{
	return m_column;
}

int Context::token()
{
	return m_token;
}

QString Context::translationUnit()
{
	return m_translationUnit;
}


void Context::setTranslationUnit(QString tu)
{
	if("0"==tu){
		qWarning()<<"ERROR: translation unit was set to "<<tu;
	}
	m_translationUnit=tu;
	m_valid=true;
}


void Context::incLine()
{
	m_line++;
	m_token=0;
	m_column=0;
	m_valid=true;
}
void Context::incColumn()
{
	m_column++;
	m_valid=true;
}

void Context::incToken()
{
	m_token++;
	m_valid=true;
}


bool Context::isValid()
{
	return m_valid;
}

QString Context::toString()
{
	QString out;
	if(!m_valid){
		return "invalid";
	}
	if(!m_translationUnit.isEmpty()){
		out+=" translationUnit " +m_translationUnit;
	}
	out+=" line "+QString::number(m_line)+" column "+QString::number(m_column)+" near token "+QString::number(m_column);
	return out;
}

