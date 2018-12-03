
/* =====[ ESPNOW for QCAN2 ]====================================

   File Name:       qcan_inter.h 
   
   Description:     Main functions for QCAN

   Revisions:

      REV   DATE            BY              DESCRIPTION
      ----  -----------     ----------      ------------------------------
      0.00  nov.29.2018     Peter Glen      Initial version.

   ======================================================================= */
 

void    initparser();
int     interpret(char *comm);

void    get_term_str(char *cline, int maxlen);



// EOF
 


