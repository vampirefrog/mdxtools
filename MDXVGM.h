#ifndef MDXVGM_H_
#define MDXVGM_H_

#include <map>

class MDXVGM: public MDX {
	std::map <uint8_t, MDXVoice> voices;

public:
	virtual void handleVoice(MDXVoice &v) {
		voices[v.number] = v;
	}
};

#endif /* MDXVGM_H_ */
