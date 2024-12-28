#include "Tokenizer.hpp"

#include "Response.hpp"
#include <cctype>




QSharedPointer<Response> Tokenizer::tokenize(const QString &source, QVector<Token> &tokens) {
	tokens.clear();
	
	int length = source.length();
	int index = 0;
	int parenthesesBalance = 0; // Track parentheses balance
	
	while (index < length) {
		QChar current = source[index];
		
		// Skip whitespace
		if (current.isSpace()) {
			int start = index;
			while (index < length && source[index].isSpace()) {
				++index;
			}
			tokens.append({TokenType::Whitespace, source.mid(start, index - start)});
			continue;
		}
		
		// Number
		if (current.isDigit() || (current == '.' && index + 1 < length && source[index + 1].isDigit())) {
			int start = index;
			bool seenDot = false;
			
			while (index < length && (source[index].isDigit() || (!seenDot && source[index] == '.'))) {
				if (source[index] == '.') {
					seenDot = true;
				}
				++index;
			}
			
			tokens.append({TokenType::Number, source.mid(start, index - start)});
			continue;
		}
		
		// Operator
		if (QString("+-*/%^=").contains(current)) {
			tokens.append({TokenType::Operator, QString(current)});
			++index;
			continue;
		}
		
		// Parenthesis
		if (current == '(') {
			++parenthesesBalance; // Track opening parenthesis
			tokens.append({TokenType::Parenthesis, "("});
			++index;
			continue;
		}
		if (current == ')') {
			--parenthesesBalance; // Track closing parenthesis
			if (parenthesesBalance < 0) {
				return QSharedPointer<Response>::create("Mismatched closing parenthesis encountered.", 0, index);
			}
			tokens.append({TokenType::Parenthesis, ")"});
			++index;
			continue;
		}
		
		// Delimiter (comma for function arguments)
		if (current == ',') {
			tokens.append({TokenType::Delimiter, ","});
			++index;
			continue;
		}
		
		// Name
		if (current.isLetter() || current == '_') {
			int start = index;
			
			while (index < length && (source[index].isLetterOrNumber() || source[index] == '_')) {
				++index;
			}
			
			tokens.append({TokenType::Name, source.mid(start, index - start)});
			continue;
		}
		
		// Invalid token
		return QSharedPointer<Response>::create(QString("Invalid character in input: '%1'").arg(current), 0, index);
	}
	
	if (parenthesesBalance != 0) {
		return QSharedPointer<Response>::create("Mismatched parentheses.");
	}
	
	tokens.append({TokenType::End, QString()});
	return QSharedPointer<Response>::create();
}



