#ifndef MML_H_
#define MML_H_

#include <stdint.h>
#include <ctype.h>

#include "exceptionf.h"
#include "Stream.h"
#include "FS.h"
#include "tools.h"

#define MML_TOKEN_TYPES \
	MML_TOKEN_TYPE(None) \
	MML_TOKEN_TYPE(Whitespace) \
	MML_TOKEN_TYPE(Newline) \
	\
	/* Directives */ \
	MML_TOKEN_TYPE(DirectiveName) \
	MML_TOKEN_TYPE(StringLiteral) \
	\
	/* Voice defs */ \
	MML_TOKEN_TYPE(At) \
	MML_TOKEN_TYPE(IntLiteral) \
	MML_TOKEN_TYPE(Equals) \
	MML_TOKEN_TYPE(CurlyOpen) \
	MML_TOKEN_TYPE(CurlyClose) \
	MML_TOKEN_TYPE(Comma) \
	\
	/* MML commands */ \
	MML_TOKEN_TYPE(Channel) \
	MML_TOKEN_TYPE(Note) \
	MML_TOKEN_TYPE(Rest) \
	MML_TOKEN_TYPE(Dot) \
	MML_TOKEN_TYPE(Tempo) \
	MML_TOKEN_TYPE(OPMTempo) \
	MML_TOKEN_TYPE(Volume) \
	MML_TOKEN_TYPE(FineVolume) \
	MML_TOKEN_TYPE(Pan) \
	MML_TOKEN_TYPE(Octave) \
	MML_TOKEN_TYPE(OctaveDown) \
	MML_TOKEN_TYPE(OctaveUp) \
	MML_TOKEN_TYPE(VolumeDown) \
	MML_TOKEN_TYPE(VolumeUp) \
	MML_TOKEN_TYPE(OPMStaccato) \
	MML_TOKEN_TYPE(MD) \
	MML_TOKEN_TYPE(MP) \
	MML_TOKEN_TYPE(MA) \
	MML_TOKEN_TYPE(MPON) \
	MML_TOKEN_TYPE(MPOF) \
	MML_TOKEN_TYPE(MAON) \
	MML_TOKEN_TYPE(MAOF) \
	MML_TOKEN_TYPE(MH) \
	MML_TOKEN_TYPE(MHON) \
	MML_TOKEN_TYPE(MHOF) \
	MML_TOKEN_TYPE(Portamento) \
	MML_TOKEN_TYPE(Legato) \
	MML_TOKEN_TYPE(Plus) \
	MML_TOKEN_TYPE(Minus) \
	MML_TOKEN_TYPE(Percent) \
	MML_TOKEN_TYPE(Ignore) \
	MML_TOKEN_TYPE(LoopStart) \
	MML_TOKEN_TYPE(LoopSkip) \
	MML_TOKEN_TYPE(A) \
	MML_TOKEN_TYPE(B) \
	MML_TOKEN_TYPE(C) \
	MML_TOKEN_TYPE(D) \
	MML_TOKEN_TYPE(E) \
	MML_TOKEN_TYPE(F) \
	MML_TOKEN_TYPE(G) \
	MML_TOKEN_TYPE(H) \
	MML_TOKEN_TYPE(P) \
	MML_TOKEN_TYPE(Q) \
	MML_TOKEN_TYPE(R) \
	MML_TOKEN_TYPE(S) \
	MML_TOKEN_TYPE(T) \
	MML_TOKEN_TYPE(U) \
	MML_TOKEN_TYPE(V) \
	MML_TOKEN_TYPE(W) \
	MML_TOKEN_TYPE(NumericNote) \
	MML_TOKEN_TYPE(Staccato) \
	MML_TOKEN_TYPE(RepeatStart) \
	MML_TOKEN_TYPE(RepeatEnd) \
	MML_TOKEN_TYPE(OPMWrite) \
	MML_TOKEN_TYPE(NoteDelay) \
	MML_TOKEN_TYPE(OPMNoiseFreq)


enum MMLTokenType {
#define MML_TOKEN_TYPE(t) Token##t,
	MML_TOKEN_TYPES
#undef MML_TOKEN_TYPE
};

struct MMLToken {
	MMLTokenType type;
	union {
		int i;
		char *s;
	} val;
	unsigned int lineNum, charNum;

	static const char *getTypeName(MMLTokenType t) {
		switch(t) {
#define MML_TOKEN_TYPE(type) case Token##type: return #type;
			MML_TOKEN_TYPES
#undef MML_TOKEN_TYPE
		}
		return "Unknown";
	}

	void dump() {
		char *ic;
		printf("%d:%d: %s", lineNum, charNum, getTypeName(type));
		switch(type) {
			case TokenDirectiveName:
			case TokenStringLiteral:
				ic = iconvAlloc(val.s, "UTF-8", "Shift_JIS");
				printf(": \"%s\"", ic);
				free(ic);
				break;
			case TokenIntLiteral:
				printf(": %d", val.i);
				break;
			case TokenNote:
			case TokenChannel:
				printf(": %c", val.i);
				break;
			default:
				break;
		}
		printf("\n");
	}
};

#define MML_TOKENIZER_STATES \
	MML_TOKENIZER_STATE(None) \
	MML_TOKENIZER_STATE(InWhitespace) \
	MML_TOKENIZER_STATE(InSlash) \
	MML_TOKENIZER_STATE(InComment) \
	MML_TOKENIZER_STATE(InCommentAsterisk) \
	MML_TOKENIZER_STATE(DirectiveName) \
	MML_TOKENIZER_STATE(StringLiteral) \
	MML_TOKENIZER_STATE(IntLiteral) \
	MML_TOKENIZER_STATE(InAt) \
	MML_TOKENIZER_STATE(InM) \
	MML_TOKENIZER_STATE(InMP) \
	MML_TOKENIZER_STATE(InMPO) \
	MML_TOKENIZER_STATE(InMA) \
	MML_TOKENIZER_STATE(InMAO) \
	MML_TOKENIZER_STATE(InMH) \
	MML_TOKENIZER_STATE(InMHO)

enum MMLTokenizerState {
#define MML_TOKENIZER_STATE(s) TokState##s,
	MML_TOKENIZER_STATES
#undef MML_TOKENIZER_STATE
};

#define BUF_APPEND(buf, ch) { if(buf##Pos < sizeof(buf) - 1) { buf[buf##Pos] = ch; buf[buf##Pos + 1] = 0; } buf##Pos++; }

class MMLTokenizer {
protected:
	MMLTokenizerState state;
	MMLToken token;
	bool inVoiceDef;
	char strBuf[256];
	unsigned int strBufPos;

	virtual void handleToken(MMLToken t) { }
public:
	int lineNum, charNum;
	MMLTokenizer():
		state(TokStateNone),
		strBufPos(0),
		lineNum(1),
		charNum(0) { }

	static const char *getTokenizerStateName(MMLTokenizerState t) {
		switch(t) {
#define MML_TOKENIZER_STATE(type) case TokState##type: return #type;
			MML_TOKENIZER_STATES
#undef MML_TOKENIZER_STATE
		}
		return "Unknown";
	}

	void eat(uint8_t b) {
		//printf("eat \"%c\" (0x%02x) state=%s (%d)\n", cleanChar(b), b, getTokenizerStateName(state), state);

		if(b == '\r') return; // just ignore it

		switch(state) {
			case TokStateNone:
				handleTokenStart(b);
				break;
			case TokStateInWhitespace:
				if(b == ' ' || b == '\t') {
				} else {
					token.type = TokenWhitespace;
					handleToken(token);
					state = TokStateNone;
					handleTokenStart(b);
				}
				break;
			case TokStateInSlash:
				if(b == '*') {
					state = TokStateInComment;
				} else {
					token.type = TokenLoopSkip;
					handleToken(token);
					handleTokenStart(b);
				}
				break;
			case TokStateInComment:
				if(b == '*') {
					state = TokStateInCommentAsterisk;
				}
				break;
			case TokStateInCommentAsterisk:
				if(b == '/') {
					state = TokStateNone;
				} else if(b == '*') {
					state = TokStateInCommentAsterisk;
				}
				break;
			case TokStateDirectiveName:
				if(!isspace(b)) {
					BUF_APPEND(strBuf, b);
				} else {
					token.type = TokenDirectiveName;
					token.val.s = strBuf;
					handleToken(token);
					handleTokenStart(b);
				}
				break;
			case TokStateStringLiteral:
				if(b == '"') {
					token.type = TokenStringLiteral;
					token.val.s = strBuf;
					handleToken(token);
					state = TokStateNone;
				} else {
					BUF_APPEND(strBuf, b);
				}
				break;
			case TokStateIntLiteral:
				if(isdigit(b)) {
					BUF_APPEND(strBuf, b);
				} else {
					token.type = TokenIntLiteral;
					token.val.i = atoi(strBuf);
					handleToken(token);
					state = TokStateNone;
					handleTokenStart(b);
				}
				break;
			case TokStateInAt:
				switch(b) {
					case 't':
						token.type = TokenOPMTempo;
						handleToken(token);
						state = TokStateNone;
						break;
					case 'v':
						token.type = TokenFineVolume;
						handleToken(token);
						state = TokStateNone;
						break;
					case 'q':
						token.type = TokenOPMStaccato;
						handleToken(token);
						state = TokStateNone;
						break;
					default:
						token.type = TokenAt;
						handleToken(token);
						handleTokenStart(b);
						break;
				}
				break;
			case TokStateInM:
				switch(b) {
					case 'P':
						state = TokStateInMP;
						break;
					case 'A':
						state = TokStateInMA;
						break;
					case 'D':
						token.type = TokenMD;
						handleToken(token);
						break;
					case 'H':
						state = TokStateInMH;
						break;
				}
				break;
			case TokStateInMP:
				if(isdigit(b) || b == '-') {
					token.type = TokenMP;
					handleToken(token);
					handleTokenStart(b);
				} else if(b == 'O') {
					state = TokStateInMPO;
				} else handleTokenStart(b);
				break;
			case TokStateInMPO:
				if(b == 'N') {
					token.type = TokenMPON;
					handleToken(token);
				} else if(b == 'F') {
					token.type = TokenMPOF;
					handleToken(token);
				} else throw exceptionf("Unexpected character '%c' (0x%02f) after 'MPO'", cleanChar(b), b);
				break;
			case TokStateInMA:
				if(isdigit(b) || b == '-') {
					token.type = TokenMA;
					handleToken(token);
					handleTokenStart(b);
				} else if(b == 'O') {
					state = TokStateInMAO;
				} else handleTokenStart(b);
				break;
			case TokStateInMAO:
				if(b == 'N') {
					token.type = TokenMAON;
					handleToken(token);
				} else if(b == 'F') {
					token.type = TokenMAOF;
					handleToken(token);
				} else throw exceptionf("Unexpected character '%c' (0x%02f) after 'MAO'", cleanChar(b), b);
				break;
			case TokStateInMH:
				if(isdigit(b) || b == '-') {
					token.type = TokenMH;
					handleToken(token);
					handleTokenStart(b);
				} else if(b == 'O') {
					state = TokStateInMHO;
				} else handleTokenStart(b);
				break;
			case TokStateInMHO:
				if(b == 'N') {
					token.type = TokenMHON;
					handleToken(token);
				} else if(b == 'F') {
					token.type = TokenMHOF;
					handleToken(token);
				} else throw exceptionf("Unexpected character '%c' (0x%02f) after 'MHO'", cleanChar(b), b);
				break;
		}

		if(b == '\n') {
			lineNum++;
			charNum = 0;
		}
		charNum++;

	}

private:
	void handleTokenStart(uint8_t b) {
		token.lineNum = lineNum;
		token.charNum = charNum;
		state = TokStateNone;

		if(b == '/' && state != TokStateInComment && state != TokStateInCommentAsterisk) {
			state = TokStateInSlash;
		} else if(isdigit(b) || b == '-') {
			state = TokStateIntLiteral;
			strBufPos = 0;
			BUF_APPEND(strBuf, b);
		} else if(b >= 'a' && b <= 'g') {
			token.type = TokenNote;
			token.val.i = b;
			handleToken(token);
			state = TokStateNone;
		} switch(b) {
			case ' ':
			case '\t':
				state = TokStateInWhitespace;
				break;
			case '#':
				state = TokStateDirectiveName;
				strBufPos = 0;
				break;
			case '"':
				state = TokStateStringLiteral;
				strBufPos = 0;
				break;
			case '@':
				state = TokStateInAt;
				break;
			case 'M':
				state = TokStateInM;
				break;

			case '\n':token.type = TokenNewline;      handleToken(token); break;
			case '{': token.type = TokenCurlyOpen;    handleToken(token); break;
			case '}': token.type = TokenCurlyClose;   handleToken(token); break;
			case '=': token.type = TokenEquals;       handleToken(token); break;
			case ',': token.type = TokenComma;        handleToken(token); break;
			case 't': token.type = TokenTempo;        handleToken(token); break;
			case 'o': token.type = TokenOctave;       handleToken(token); break;
			case '(': token.type = TokenVolumeDown;   handleToken(token); break;
			case ')': token.type = TokenVolumeUp;     handleToken(token); break;
			case '_': token.type = TokenPortamento;   handleToken(token); break;
			case '&': token.type = TokenLegato;       handleToken(token); break;
			case '.': token.type = TokenDot;          handleToken(token); break;
			case '+': token.type = TokenPlus;         handleToken(token); break;
			case '-': token.type = TokenMinus;        handleToken(token); break;
			case '%': token.type = TokenPercent;      handleToken(token); break;
			case '<': token.type = TokenOctaveDown;   handleToken(token); break;
			case '>': token.type = TokenOctaveUp;     handleToken(token); break;
			case '!': token.type = TokenIgnore;       handleToken(token); break;
			case 'L': token.type = TokenLoopStart;    handleToken(token); break;
			case 'n': token.type = TokenNumericNote;  handleToken(token); break;
			case 'r': token.type = TokenRest;         handleToken(token); break;
			case 'q': token.type = TokenStaccato;     handleToken(token); break;
			case 'v': token.type = TokenVolume;       handleToken(token); break;
			case 'p': token.type = TokenPan;          handleToken(token); break;
			case '[': token.type = TokenRepeatStart;  handleToken(token); break;
			case ']': token.type = TokenRepeatEnd;    handleToken(token); break;
			case 'y': token.type = TokenOPMWrite;     handleToken(token); break;
			case 'k': token.type = TokenNoteDelay;    handleToken(token); break;
			case 'w': token.type = TokenOPMNoiseFreq; handleToken(token); break;
			case 'A': token.type = TokenA;            handleToken(token); break;
			case 'B': token.type = TokenB;            handleToken(token); break;
			case 'C': token.type = TokenC;            handleToken(token); break;
			case 'D': token.type = TokenD;            handleToken(token); break;
			case 'E': token.type = TokenE;            handleToken(token); break;
			case 'F': token.type = TokenF;            handleToken(token); break;
			case 'G': token.type = TokenG;            handleToken(token); break;
			case 'H': token.type = TokenH;            handleToken(token); break;
			case 'P': token.type = TokenP;            handleToken(token); break;
			case 'Q': token.type = TokenQ;            handleToken(token); break;
			case 'R': token.type = TokenR;            handleToken(token); break;
			case 'S': token.type = TokenS;            handleToken(token); break;
			case 'T': token.type = TokenT;            handleToken(token); break;
			case 'U': token.type = TokenU;            handleToken(token); break;
			case 'V': token.type = TokenV;            handleToken(token); break;
			case 'W': token.type = TokenW;            handleToken(token); break;
		}
	}
};

class MMLParser;
class MMLParserTokenizer: public MMLTokenizer {
public:
	MMLParser *parent;
	void handleToken(MMLToken token);
};

#define MML_PARSER_STATES \
	MML_PARSER_STATE(None) \
	MML_PARSER_STATE(AfterDirectiveName) \
	MML_PARSER_STATE(AfterDirectiveValue) \
	MML_PARSER_STATE(InVoiceDefNum) \
	MML_PARSER_STATE(AfterVoiceDefNum) \
	MML_PARSER_STATE(AfterVoiceDefEquals) \
	MML_PARSER_STATE(InVoiceDef) \
	MML_PARSER_STATE(AfterVoiceDefIntLiteral) \
	MML_PARSER_STATE(AfterVoiceDef) \
	MML_PARSER_STATE(InChannelID) \
	MML_PARSER_STATE(InMMLData) \
	MML_PARSER_STATE(InVolume) \
	MML_PARSER_STATE(InOPMTempo) \
	MML_PARSER_STATE(InTempo) \
	MML_PARSER_STATE(InSetVoice) \
	MML_PARSER_STATE(InOctave) \
	MML_PARSER_STATE(InDetune) \
	MML_PARSER_STATE(InPan) \
	MML_PARSER_STATE(InRest) \
	MML_PARSER_STATE(AfterNote)

enum MMLParserState {
#define MML_PARSER_STATE(st) ParserState##st,
	MML_PARSER_STATES
#undef MML_PARSER_STATE
};

#define MML_PARSER_CMDS \
	MML_PARSER_CMD(None) \
	MML_PARSER_CMD(Note) \
	MML_PARSER_CMD(Tempo) \
	MML_PARSER_CMD(OPMTempo) \
	MML_PARSER_CMD(Octave) \
	MML_PARSER_CMD(NoteLength) \
	MML_PARSER_CMD(NumericNote) \
	MML_PARSER_CMD(Rest) \
	MML_PARSER_CMD(Staccato) \
	MML_PARSER_CMD(OPMStaccato) \
	MML_PARSER_CMD(Volume) \
	MML_PARSER_CMD(OPMVolume) \
	MML_PARSER_CMD(Pan) \
	MML_PARSER_CMD(Detune) \
	MML_PARSER_CMD(LoopEnd) \
	MML_PARSER_CMD(OPMWrite) \
	MML_PARSER_CMD(NoteDelay) \
	MML_PARSER_CMD(OPMNoiseFreq) \
	MML_PARSER_CMD(SyncSend) \
	MML_PARSER_CMD(PCMFreq) \
	MML_PARSER_CMD(SetVoice) \
	MML_PARSER_CMD(VoiceDefinition) \
	MML_PARSER_CMD(VoiceDefinitionBody) \
	MML_PARSER_CMD(MH)

enum MMLParserCommand {
#define MML_PARSER_CMD(c) ParserCmd##c,
		MML_PARSER_CMDS
#undef MML_PARSER_CMD
};

#define CHANNEL_MASK_A (1 << 0)
#define CHANNEL_MASK_B (1 << 1)
#define CHANNEL_MASK_C (1 << 2)
#define CHANNEL_MASK_D (1 << 3)
#define CHANNEL_MASK_E (1 << 4)
#define CHANNEL_MASK_F (1 << 5)
#define CHANNEL_MASK_G (1 << 6)
#define CHANNEL_MASK_H (1 << 7)
#define CHANNEL_MASK_P (1 << 8)
#define CHANNEL_MASK_Q (1 << 9)
#define CHANNEL_MASK_R (1 << 10)
#define CHANNEL_MASK_S (1 << 11)
#define CHANNEL_MASK_T (1 << 12)
#define CHANNEL_MASK_U (1 << 13)
#define CHANNEL_MASK_V (1 << 14)
#define CHANNEL_MASK_W (1 << 15)

#define NOTE_FLAG_SHARP (1 << 0)
#define NOTE_FLAG_FLAT (1 << 1)
#define NOTE_FLAG_DOTTED (1 << 2)
#define NOTE_FLAG_OPM_LENGTH (1 << 3)

class MMLParser {
protected:
	MMLParserTokenizer tok;
	MMLParserState state;

private:
	// values
	char directiveName[256], directiveValue[256];
	int curVoiceNum, curVoiceParam;
	int voiceData[47];
	uint32_t channelMask;
	int note, noteLength, noteFlags;

protected:
	virtual void handleError(const char* file, int line, const char *fmt, ...) {}

	// Directives
	virtual void handleDirective(char *directive, char *value) {}

	// Voice data
	virtual void handleVoice(int voiceNum, int *voiceData) {}

	// MML commands
	virtual void handleVolume(uint32_t channelMask, int volume) {}
	virtual void handleOPMTempo(uint32_t channelMask, int tempo) {}
	virtual void handleTempo(uint32_t channelMask, int tempo) {}
	virtual void handleSetVoice(uint32_t channelMask, int voiceNum) {}
	virtual void handleDetune(uint32_t channelMask, int detune) {}
	virtual void handlePan(uint32_t channelMask, int pan) {}
	virtual void handleRest(uint32_t channelMask, int length) {}
	virtual void handleOctave(uint32_t channelMask, int octave) {}
	virtual void handleOctaveDown(uint32_t channelMask) {}
	virtual void handleOctaveUp(uint32_t channelMask) {}
	virtual void handleLegato(uint32_t channelMask) {}
	virtual void handleNote(uint32_t channelMask, int note, int noteLength, uint32_t noteFlags) {}
	virtual void handleRest(uint32_t channelMask, int noteLength, uint32_t noteFlags) {}

public:
	MMLParser(): state(ParserStateNone) {
		tok.parent = this;
	}
	void eat(uint8_t b) {
		tok.eat(b);
	}
	const char *getStateName() {
		switch(state) {
#define MML_PARSER_STATE(st) case ParserState##st: return #st;
			MML_PARSER_STATES;
#undef MML_PARSER_STATE
		}
		return "Unknown";
	}
	void handleToken(MMLToken token) {
		// printf("handleToken %s\n", MMLToken::getTypeName(token.type));
		switch(state) {
			case ParserStateNone:
				switch(token.type) {
					case TokenWhitespace:
					case TokenNewline:
						// ignore
						break;
					case TokenAt:
						state = ParserStateInVoiceDefNum;
						break;
					case TokenA: channelMask = CHANNEL_MASK_A; state = ParserStateInChannelID; break;
					case TokenB: channelMask = CHANNEL_MASK_B; state = ParserStateInChannelID; break;
					case TokenC: channelMask = CHANNEL_MASK_C; state = ParserStateInChannelID; break;
					case TokenD: channelMask = CHANNEL_MASK_D; state = ParserStateInChannelID; break;
					case TokenE: channelMask = CHANNEL_MASK_E; state = ParserStateInChannelID; break;
					case TokenF: channelMask = CHANNEL_MASK_F; state = ParserStateInChannelID; break;
					case TokenG: channelMask = CHANNEL_MASK_G; state = ParserStateInChannelID; break;
					case TokenH: channelMask = CHANNEL_MASK_H; state = ParserStateInChannelID; break;
					case TokenP: channelMask = CHANNEL_MASK_P; state = ParserStateInChannelID; break;
					case TokenQ: channelMask = CHANNEL_MASK_Q; state = ParserStateInChannelID; break;
					case TokenR: channelMask = CHANNEL_MASK_R; state = ParserStateInChannelID; break;
					case TokenS: channelMask = CHANNEL_MASK_S; state = ParserStateInChannelID; break;
					case TokenT: channelMask = CHANNEL_MASK_T; state = ParserStateInChannelID; break;
					case TokenU: channelMask = CHANNEL_MASK_U; state = ParserStateInChannelID; break;
					case TokenV: channelMask = CHANNEL_MASK_V; state = ParserStateInChannelID; break;
					case TokenW: channelMask = CHANNEL_MASK_W; state = ParserStateInChannelID; break;
					case TokenDirectiveName:
						strncpy(directiveName, token.val.s, sizeof(directiveName) - 1);
						directiveName[sizeof(directiveName) - 1] = 0;
						state = ParserStateAfterDirectiveName;
						break;
					default:
						handleError(__FILE__, __LINE__, "Unexpected token %s", MMLToken::getTypeName(token.type));
						break;
				}
				break;
			case ParserStateAfterDirectiveName:
				switch(token.type) {
					case TokenWhitespace:
						// ignore
						break;
					case TokenStringLiteral:
						handleDirective(directiveName, token.val.s);
						state = ParserStateNone;
						break;
					default:
						handleError(__FILE__, __LINE__, "Unexpected token %s", MMLToken::getTypeName(token.type));
						break;
				}
				break;
			case ParserStateAfterDirectiveValue:
				switch(token.type) {
					case TokenWhitespace:
						// ignore
						break;
					case TokenNewline:
						state = ParserStateNone;
						break;
					default:
						handleError(__FILE__, __LINE__, "Unexpected token %s", MMLToken::getTypeName(token.type));
						break;
				}
				break;
			case ParserStateInVoiceDefNum:
				if(token.type == TokenIntLiteral) {
					curVoiceNum = token.val.i;
					state = ParserStateAfterVoiceDefNum;
				} else {
					handleError(__FILE__, __LINE__, "Unexpected token %s", MMLToken::getTypeName(token.type));
				}
				break;
			case ParserStateAfterVoiceDefNum:
				if(token.type == TokenWhitespace || token.type == TokenNewline) {
					// ignore
				} else if(token.type == TokenEquals) {
					state = ParserStateAfterVoiceDefEquals;
				} else {
					handleError(__FILE__, __LINE__, "Unexpected token %s", MMLToken::getTypeName(token.type));
				}
				break;
			case ParserStateAfterVoiceDefEquals:
				if(token.type == TokenWhitespace || token.type == TokenNewline) {
					// ignore
				} else if(token.type == TokenCurlyOpen) {
					state = ParserStateInVoiceDef;
					curVoiceParam = 0;
				} else {
					handleError(__FILE__, __LINE__, "Unexpected token %s", MMLToken::getTypeName(token.type));
				}
				break;
			case ParserStateInVoiceDef:
				if(token.type == TokenWhitespace || token.type == TokenNewline) {
					// ignore
				} else if(token.type == TokenIntLiteral) {
					if(curVoiceParam < 47) {
						voiceData[curVoiceParam++] = token.val.i;
						state = ParserStateAfterVoiceDefIntLiteral;
					} else {
						handleError(__FILE__, __LINE__, "Unexpected token %s", MMLToken::getTypeName(token.type));
					}
				} else {
					handleError(__FILE__, __LINE__, "Unexpected token %s", MMLToken::getTypeName(token.type));
				}
				break;
			case ParserStateAfterVoiceDefIntLiteral:
				if(token.type == TokenWhitespace || token.type == TokenNewline) {
					// ignore
				} else if(token.type == TokenComma) {
					if(curVoiceParam < 47) {
						state = ParserStateInVoiceDef;
					} else {
						handleError(__FILE__, __LINE__, "Unexpected token %s", MMLToken::getTypeName(token.type));
					}
				} else if(token.type == TokenCurlyClose) {
					if(curVoiceParam == 47) {
						handleVoice(curVoiceNum, voiceData);
						state = ParserStateAfterVoiceDef;
					} else handleError(__FILE__, __LINE__, "Unexpected token %s", MMLToken::getTypeName(token.type));
				} else {
					handleError(__FILE__, __LINE__, "Unexpected token %s curVoiceParam=%d", MMLToken::getTypeName(token.type), curVoiceParam);
				}
				break;
			case ParserStateAfterVoiceDef:
				if(token.type == TokenWhitespace) {
					// ignore
				} else if(token.type == TokenNewline) {
					state = ParserStateNone;
				} else {
					handleError(__FILE__, __LINE__, "Unexpected token %s", MMLToken::getTypeName(token.type));
				}
				break;
			case ParserStateInChannelID:
				switch(token.type) {
					case TokenA: channelMask |= CHANNEL_MASK_A; break;
					case TokenB: channelMask |= CHANNEL_MASK_B; break;
					case TokenC: channelMask |= CHANNEL_MASK_C; break;
					case TokenD: channelMask |= CHANNEL_MASK_D; break;
					case TokenE: channelMask |= CHANNEL_MASK_E; break;
					case TokenF: channelMask |= CHANNEL_MASK_F; break;
					case TokenG: channelMask |= CHANNEL_MASK_G; break;
					case TokenH: channelMask |= CHANNEL_MASK_H; break;
					case TokenP: channelMask |= CHANNEL_MASK_P; break;
					case TokenQ: channelMask |= CHANNEL_MASK_Q; break;
					case TokenR: channelMask |= CHANNEL_MASK_R; break;
					case TokenS: channelMask |= CHANNEL_MASK_S; break;
					case TokenT: channelMask |= CHANNEL_MASK_T; break;
					case TokenU: channelMask |= CHANNEL_MASK_U; break;
					case TokenV: channelMask |= CHANNEL_MASK_V; break;
					case TokenW: channelMask |= CHANNEL_MASK_W; break;
					case TokenWhitespace:
						state = ParserStateInMMLData;
						break;
					case TokenNewline:
						state = ParserStateNone;
						break;
					default:
						handleError(__FILE__, __LINE__, "Unexpected token %s", MMLToken::getTypeName(token.type));
				}
				break;
			case ParserStateInMMLData:
				handleMMLToken(token);
				break;
			case ParserStateAfterNote:
				switch(token.type) {
					case TokenIntLiteral:
						noteLength = token.val.i;
						break;
					case TokenPlus:
						noteFlags |= NOTE_FLAG_SHARP;
						break;
					case TokenMinus:
						noteFlags |= NOTE_FLAG_FLAT;
						break;
					case TokenDot:
						noteFlags |= NOTE_FLAG_DOTTED;
						break;
					case TokenPercent:
						noteFlags |= NOTE_FLAG_OPM_LENGTH;
						break;
					default:
						handleNote(channelMask, note, noteLength, noteFlags);
						state = ParserStateInMMLData;
						handleMMLToken(token);
						break;
				}
				break;
			case ParserStateInRest:
				switch(token.type) {
					case TokenIntLiteral:
						noteLength = token.val.i;
						break;
					case TokenDot:
						noteFlags |= NOTE_FLAG_DOTTED;
						break;
					case TokenPercent:
						noteFlags |= NOTE_FLAG_OPM_LENGTH;
						break;
					default:
						handleRest(channelMask, noteLength, noteFlags);
						state = ParserStateInMMLData;
						handleMMLToken(token);
						break;
				}
				break;

			case ParserStateInVolume:   if(token.type == TokenIntLiteral) { handleVolume  (channelMask, token.val.i); state = ParserStateInMMLData; } else { handleError(__FILE__, __LINE__, "Unexpected token %s", MMLToken::getTypeName(token.type)); } break;
			case ParserStateInOPMTempo: if(token.type == TokenIntLiteral) { handleOPMTempo(channelMask, token.val.i); state = ParserStateInMMLData; } else { handleError(__FILE__, __LINE__, "Unexpected token %s", MMLToken::getTypeName(token.type)); } break;
			case ParserStateInTempo:    if(token.type == TokenIntLiteral) { handleTempo   (channelMask, token.val.i); state = ParserStateInMMLData; } else { handleError(__FILE__, __LINE__, "Unexpected token %s", MMLToken::getTypeName(token.type)); } break;
			case ParserStateInSetVoice: if(token.type == TokenIntLiteral) { handleSetVoice(channelMask, token.val.i); state = ParserStateInMMLData; } else { handleError(__FILE__, __LINE__, "Unexpected token %s", MMLToken::getTypeName(token.type)); } break;
			case ParserStateInDetune:   if(token.type == TokenIntLiteral) { handleDetune  (channelMask, token.val.i); state = ParserStateInMMLData; } else { handleError(__FILE__, __LINE__, "Unexpected token %s", MMLToken::getTypeName(token.type)); } break;
			case ParserStateInPan:      if(token.type == TokenIntLiteral) { handlePan     (channelMask, token.val.i); state = ParserStateInMMLData; } else { handleError(__FILE__, __LINE__, "Unexpected token %s", MMLToken::getTypeName(token.type)); } break;
			case ParserStateInOctave:   if(token.type == TokenIntLiteral) { handleOctave  (channelMask, token.val.i); state = ParserStateInMMLData; } else { handleError(__FILE__, __LINE__, "Unexpected token %s", MMLToken::getTypeName(token.type)); } break;
		}
	}
private:
	void handleMMLToken(MMLToken token) {
		switch(token.type) {
			case TokenWhitespace:
				// ignore
				break;
			case TokenNewline:
				state = ParserStateNone;
				break;
			case TokenNote:
				note = token.val.i;
				noteLength = 0;
				noteFlags = 0;
				state = ParserStateAfterNote;
				break;
			case TokenRest:
				noteLength = 0;
				noteFlags = 0;
				state = ParserStateInRest;
				break;
			case TokenVolume:   state = ParserStateInVolume;   break;
			case TokenOPMTempo: state = ParserStateInOPMTempo; break;
			case TokenTempo:    state = ParserStateInTempo; break;
			case TokenAt:       state = ParserStateInSetVoice; break;
			case TokenD:        state = ParserStateInDetune;   break;
			case TokenPan:      state = ParserStateInPan;      break;
			case TokenOctave:   state = ParserStateInOctave;   break;

			case TokenOctaveDown: handleOctaveDown(channelMask); state = ParserStateInMMLData; break;
			case TokenOctaveUp:   handleOctaveUp  (channelMask); state = ParserStateInMMLData; break;
			case TokenLegato:     handleLegato    (channelMask); state = ParserStateInMMLData; break;
			default:
				handleError(__FILE__, __LINE__, "Unexpected token %s", MMLToken::getTypeName(token.type));
				break;
		}
	}
};

#endif /* MML_H_ */
