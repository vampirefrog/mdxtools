struct adpcm_status {
    short last;
    short step_index;
};

void adpcm_init(struct adpcm_status *);
char adpcm_encode( short, struct adpcm_status *);
short adpcm_decode( char, struct adpcm_status *);
