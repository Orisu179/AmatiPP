
/*
  ==============================================================================

    FaustCodeTokenizer.cpp
    Author:  Oliver Larkin
    Modified by: Grégoire Locqueville (2020): changed color palette

    ==============================================================================
*/


#include "FaustTokeniser.h"

struct FaustTokeniserFunctions
{
    static bool isPrimitive (juce::String::CharPointerType token, const int tokenLength) noexcept
    {
        static const char* const primitives3Char[] =
            { "mem", "int", "cos", "sin", "tan", "exp", "log", "pow", "abs", "min", "max", "seq", "par", "sum", nullptr };

        static const char* const primitives4Char[] =
            { "acos", "asin", "atan", "fmod", "ceil", "rint", "sqrt", "with", "case", "prod", nullptr };

        static const char* const primitives5Char[] =
            {  "float", "log10", "floor", "atan2", nullptr };

        static const char* const primitives6Char[] =
            { "prefix", "button", "nentry", "vgroup", "hgroup", "tgroup", "attach", "import", nullptr};

        static const char* const primitives7Char[] =
            { "rdtable", "rwtable", "select2", "select3", "vslider", "hslider", "process", "library", "declare", nullptr};

        static const char* const primitivesOther[] =
            { "remainder", "checkbox", "vbargraph", "hbargraph", "ffunction", "fconstant", "fvariable", "component", "environment", nullptr };

        const char* const* k;

        switch (tokenLength)
        {
            case 3:   k = primitives3Char; break;
            case 4:   k = primitives4Char; break;
            case 5:   k = primitives5Char; break;
            case 6:   k = primitives6Char; break;
            case 7:   k = primitives7Char; break;

            default:
                if (tokenLength < 3 || tokenLength > 9)
                    return false;

                k = primitivesOther;
                break;
        }

        for (int i = 0; k[i] != nullptr; ++i)
            if (token.compare (juce::CharPointer_ASCII (k[i])) == 0)
                return true;

        return false;
    }

    //  static bool isOperator (juce::String::CharPointerType token, const int tokenLength) noexcept
    //  {
    //    static const char* const operator3Char[] =
    //    { "seq", "par", "sum", nullptr };
    //
    //    static const char* const operator4Char[] =
    //    { "with", "case", "prod", nullptr };
    //
    //    static const char* const operator6Char[] =
    //    { "import", nullptr };
    //
    //    static const char* const operator7Char[] =
    //    { "process", "library", "declare", nullptr };
    //
    //    static const char* const operatorOther[] =
    //    { "component", "environment", nullptr };
    //
    //    const char* const* k;
    //
    //    switch (tokenLength)
    //    {
    //      case 3:   k = operator3Char; break;
    //      case 4:   k = operator4Char; break;
    //      case 6:   k = operator6Char; break;
    //      case 7:   k = operator7Char; break;
    //
    //      default:
    //        if (tokenLength < 3 || tokenLength > 11)
    //          return false;
    //
    //        k = operatorOther;
    //        break;
    //    }
    //
    //    for (int i = 0; k[i] != 0; ++i)
    //      if (token.compare (CharPointer_ASCII (k[i])) == 0)
    //        return true;
    //
    //    return false;
    //  }

    template <typename Iterator>
    static int parseIdentifier (Iterator& source) noexcept
    {
        int tokenLength = 0;
        juce::String::CharPointerType::CharType possibleIdentifier [100];
        juce::String::CharPointerType possible (possibleIdentifier);

        while (juce::CppTokeniserFunctions::isIdentifierBody (source.peekNextChar()))
        {
            const juce::juce_wchar c = source.nextChar();

            if (tokenLength < 20)
                possible.write (c);

            ++tokenLength;
        }

        if (tokenLength > 1 && tokenLength <= 16)
        {
            possible.writeNull();

            if (isPrimitive (juce::String::CharPointerType (possibleIdentifier), tokenLength))
                return FaustTokeniser::tokenType_primitive;

            //      if (isOperator(juce::String::CharPointerType (possibleIdentifier), tokenLength))
            //        return FaustTokeniser::tokenType_operator;
        }

        return FaustTokeniser::tokenType_identifier;
    }

    template <typename Iterator>
    static int readNextToken (Iterator& source)
    {
        source.skipWhitespace();

        const juce::juce_wchar firstChar = source.peekNextChar();

        switch (firstChar)
        {
            case 0:
                break;

            case '0':   case '1':   case '2':   case '3':   case '4':
            case '5':   case '6':   case '7':   case '8':   case '9':
            case '.':
            {
                int result = juce::CppTokeniserFunctions::parseNumber (source);

                if (result == FaustTokeniser::tokenType_error)
                {
                    source.skip();

                    if (firstChar == '.')
                        return FaustTokeniser::tokenType_punctuation;
                }

                return result;
            }

            case ';':
                source.skip();
                return FaustTokeniser::tokenType_identifier;

            case ',':
            case ':':
                source.skip();
                return FaustTokeniser::tokenType_operator;

            case '(':   case ')':
            case '{':   case '}':
            case '[':   case ']':
                source.skip();
                return FaustTokeniser::tokenType_bracket;

            case '"':
                juce::CppTokeniserFunctions::skipQuotedString(source);
                return FaustTokeniser::tokenType_string;

            case '\'':
                source.skip();
                return FaustTokeniser::tokenType_operator;

            case '+':
                source.skip();
                juce::CppTokeniserFunctions::skipIfNextCharMatches (source, '+', '=');
                return FaustTokeniser::tokenType_operator;

            case '-':
                source.skip();
                return FaustTokeniser::tokenType_operator;

            case '/':
            {
                source.skip();
                juce::juce_wchar nextChar = source.peekNextChar();

                if (nextChar == '/')
                {
                    source.skipToEndOfLine();
                    return FaustTokeniser::tokenType_comment;
                }

                if (nextChar == '*')
                {
                    source.skip();
                    juce::CppTokeniserFunctions::skipComment (source);
                    return FaustTokeniser::tokenType_comment;
                }

                if (nextChar == '=')
                    source.skip();

                return juce::CPlusPlusCodeTokeniser::tokenType_operator;
            }

            case '*':   case '%':
            case '=':   case '!':
                source.skip();
                juce::CppTokeniserFunctions::skipIfNextCharMatches (source, '=');
                return FaustTokeniser::tokenType_operator;

            case '?':
            case '~':
                source.skip();
                return FaustTokeniser::tokenType_operator;

            case '<':   case '>':
            case '|':   case '&':   case '^':
                source.skip();
                juce::CppTokeniserFunctions::skipIfNextCharMatches (source, firstChar);
                juce::CppTokeniserFunctions::skipIfNextCharMatches (source, '=');
                return FaustTokeniser::tokenType_operator;

            default:
                if (juce::CppTokeniserFunctions::isIdentifierStart (firstChar))
                    return parseIdentifier (source);

                source.skip();
                break;
        }

        return FaustTokeniser::tokenType_error;
    }
};

//==============================================================================
FaustTokeniser::FaustTokeniser() = default;
FaustTokeniser::~FaustTokeniser() = default;

int FaustTokeniser::readNextToken (juce::CodeDocument::Iterator& source)
{
    return FaustTokeniserFunctions::readNextToken (source);
}

juce::CodeEditorComponent::ColourScheme FaustTokeniser::getDefaultColourScheme()
{
    Colours& globalColours = Colours::getInstance();

    static const juce::CodeEditorComponent::ColourScheme::TokenType types[] = {
        { "Error",          globalColours.getColour("oneDarkMidGrey") }, // white
        { "Comment",        globalColours.getColour("oneDarkMidGrey") },
        { "Primitive",      globalColours.getColour("oneDarkPurple") }, //orange
        { "Operator",       globalColours.getColour("oneDarkTeal") },
        { "Identifier",     globalColours.getColour("oneDarkLightGrey") },
        { "Integer",        globalColours.getColour("oneDarkYellow") },
        { "Float",          globalColours.getColour("oneDarkYellow") },
        { "String",         globalColours.getColour("oneDarkGreen") },
        { "Bracket",        globalColours.getColour("oneDarkLightGrey") },
        { "Punctuation",    globalColours.getColour("oneDarkLightGrey") }
    };
    juce::CodeEditorComponent::ColourScheme cs;

    for(unsigned int i = 0; i < sizeof (types) / sizeof (types[0]); ++i)  // (NB: numElementsInArray doesn't work here in GCC4.2)
        cs.set (types[i].name, types[i].colour);

    return cs;
}
