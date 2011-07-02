/* Public API function prototypes for the CGRAPH libraries.

Geoff Ghose, 4/18/89
Izumi Ohzawa, 96-08-28
Pathch by Alejandro Aguilar Sierra, 17 Oct 1997
_________________________________________________________________________
*/

#ifndef __CGRAPH_H__
#define __CGRAPH_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int cg_aorigin(float xpos,float ypos);
int cg_axis_enable(int axisEnable, int numberEnable);
int cg_closepath(void);
int cg_comments(char *comments);
int cg_coord_select(int marks,int text);
int cg_dash(int type,float multiplier);
int cg_fill(void);
int cg_eofill(void);
int cg_font(float fontsize, int type);
int cg_fontbyname(float fsize, char *fontname);
int cg_gsave(void);
int cg_grestore(void);
int cg_init(int rot, int expand, float scale);
void cg_launch_preview(int flag);
int cg_linax_style(int precision,int numoff,int numdist,int tnposition, int ticlen);
int cg_line(float xpos,float ypos);
int cg_linewidth(float width);
int cg_logax_style(int numsel, int ticsel, int numdist, int tnposition,
			int ticlen, int ticlen10, int numformat);
int cg_marker(float xpos,float ypos,int type,float size);
int cg_mesh(void);
int cg_move(float xpos,float ypos);
int cg_pointer(float xpos,float ypos,int type,float size);
int cg_postscript(char *command);
int cg_print(int flag);
int cg_rectfill(float xpos, float ypos, float width, float height);
int cg_reset(void);
int cg_rmove(float xpos,float ypos);
int cg_rline(float xpos,float ypos);
int cg_rorigin(float xpos,float ypos);
int cg_save(int flag);
char *cg_get_output_filename(void);
void cg_set_output_filename(char *file);
int cg_set_top_comments(char *text);
int cg_setcreator(char *ptext);
int cg_settitle(char *ptext);
int cg_setboundingbox(char *ptext);
int cg_setprolog(char *text);
int cg_setup(int gdevice);		/* obsolete, included just for compatibility */
int cg_showpage(void);
int cg_stroke(void);
int cg_text(float xpos,float ypos,float dir,char *text);
int cg_xaxis(float size,float min,float max,float offset, float ticsep,int numsep);
int cg_yaxis(float size,float min,float max,float offset, float ticsep,int numsep);
int cg_xlog(float size,float min,float max,float offset);
int cg_ylog(float size,float min,float max,float offset);
void cg_centerlabel (char *textString, float x, float y , float rotateBy, int coordSystem);
void cg_xlabel (char *textString);
void cg_ylabel (char *textString);
void cg_textalign (float x, float y,float rotate, char *textString,
		 int xpos, int ypos, int coordSystem);

/* Color specifing functions */
void cg_useflexcolor(int flag);		/* enable use flexible gray and color spec */

void cg_use_stdout(int flag);		/* send output to stdout if non-zero */

/* For Non-Marker objects */
int cg_gray(float lightness);
void cg_rgbcolor(float r, float g, float b);
void cg_grayrgbcolor(float gray, float r, float g, float b);

/* For Markers and Pointers */
void cg_markergray (float newPerimeter, float newInside);
void cg_markercolor (float peri_r, float peri_g, float peri_b,
		     float inside_r, float inside_g, float inside_b);
void cg_markergraycolor (float peri_gray, float peri_r, float peri_g, float peri_b,
		     float inside_gray, float inside_r, float inside_g, float inside_b);

/* Enable opaque background */
void cg_opaqueBG(float gray, float r, float g, float b);




/* cg_font defines */
#define HELVETICA 30
#define TIMES 10
#define COURIER	20
#define ITALIC 2
#define BOLD 1
#define BOLDITALIC 3
/* cg_coord_select defines */
#define INCHES 0
#define AXIS 1
/* cg_init defines */
#define LANDSCAPE 0
#define PORTRAIT 1
#define NOEXPAND 0
#define EXPAND 1
#define NOSCALE 0.
#define SCALE 1.
/* pointer defines */
#define PRIGHT 0
#define PDOWN 1
#define PLEFT 2
#define PUP 3
/* setup defines */
#define SCREEN 1
#define PRINTER 0

/* new for cg_print and cg_save */

#define SAVE 1
#define NOSAVE 0
#define ALLPRINT 2
#define YESNOPRINT 1
#define NOPRINT 0


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /*  __CGRAPH_H__  */


