#ifndef _tgl_zbuffer_h_
#define _tgl_zbuffer_h_

/*
 * Z buffer
 */

#include "zfeatures.h"
#include "GL/gl.h"



#define ZB_Z_BITS 16

#define ZB_POINT_Z_FRAC_BITS 14


#define ZB_POINT_S_MIN ( (1<<ZB_POINT_S_FRAC_BITS) )
#define ZB_POINT_S_MAX ( (1<<(1+TGL_FEATURE_TEXTURE_POW2+ZB_POINT_S_FRAC_BITS))-ZB_POINT_S_MIN )
#define ZB_POINT_T_MIN ( (1<<ZB_POINT_T_FRAC_BITS) )
#define ZB_POINT_T_MAX ( (1<<(1+TGL_FEATURE_TEXTURE_POW2+ZB_POINT_T_FRAC_BITS))-ZB_POINT_T_MIN )
#define ZB_POINT_S_VALUE (ZB_POINT_S_FRAC_BITS + 4)
#define ZB_POINT_T_VALUE (ZB_POINT_T_FRAC_BITS - 4)
#define ZB_S_MASK ((TGL_FEATURE_TEXTURE_DIM-1)<<(ZB_POINT_S_FRAC_BITS+1))
#define ZB_T_MASK ((TGL_FEATURE_TEXTURE_DIM-1)<<(ZB_POINT_T_FRAC_BITS+1))

#if ZB_POINT_T_FRAC_BITS == (ZB_POINT_S_FRAC_BITS + TGL_FEATURE_TEXTURE_POW2)
#define ST_TO_TEXTURE_BYTE_OFFSET(s,t) ( ((s & ZB_S_MASK) | (t & ZB_T_MASK)) >> (ZB_POINT_S_VALUE-PSZSH))
#else
#define ST_TO_TEXTURE_BYTE_OFFSET(s,t) ( ((s & ZB_S_MASK)>>(ZB_POINT_S_VALUE-PSZSH)) | ((t & ZB_T_MASK)>>(ZB_POINT_T_VALUE-PSZSH))  )
#endif

/*The corrected mult mask prevents a bug relating to color interp. it's also why the color bit depth is so damn high.*/
#define COLOR_MULT_MASK (0xff0000)
#define COLOR_CORRECTED_MULT_MASK (0xfe0000)
#define COLOR_MASK 		(0xffffff)
#define COLOR_MIN_MULT (0x00ffff)
#define COLOR_SHIFT		16

#define COLOR_R_GET32(r) ((r) & 0xff0000)
#define COLOR_G_GET32(g) ((g>>8) & 0xff00)
#define COLOR_B_GET32(b) ((b>>16)&0xff)
#define COLOR_A_GET32(a) (((a)<<8) & 0xff000000)

#define COLOR_R_GET16(r) ((r>>8) & 0xF800)
#define COLOR_G_GET16(g) ((((g)) >> 13) & 0x07E0)
#define COLOR_B_GET16(b) (((b) >> 19) & 31)
#define COLOR_A_GET16(a) (0)

#if TGL_FEATURE_RENDER_BITS == 32
#define RGB_TO_PIXEL(r,g,b,a) \
  ( COLOR_R_GET32(r) | COLOR_G_GET32(g) | COLOR_B_GET32(b) | COLOR_A_GET32(a) )
#elif TGL_FEATURE_RENDER_BITS == 16
#define RGB_TO_PIXEL(r,g,b,a) \
	( COLOR_R_GET16(r) | COLOR_G_GET16(g) | COLOR_B_GET16(b) | COLOR_B_GET16(a)  )
#endif
/*This is how textures are sampled. if you want to do some sort of fancy texture filtering,*/
/*you do it here.*/
#define TEXTURE_SAMPLE(texture, s, t)														\
 (*(PIXEL*)( (GLbyte*)texture + 															\
 ST_TO_TEXTURE_BYTE_OFFSET(s,t) 								\
 ))
/* display modes */
#define ZB_MODE_5R6G5B  1  /* true color 16 bits */
#define ZB_MODE_INDEX   2  /* color index 8 bits */
#define ZB_MODE_RGBA    3  /* 32 bit ABGR mode */
#define ZB_MODE_RGB24   4  /* 24 bit rgb mode */
#define ZB_NB_COLORS    225 /* number of colors for 8 bit display */



#define TGL_CLAMPI(imp) ( (imp>0)?((imp>COLOR_MASK)?COLOR_MASK:imp):0   )

#if TGL_FEATURE_RENDER_BITS == 32

/* 32 bit mode */

#define GET_REDDER(p) ((p & 0xff0000))
#define GET_GREENER(p) ((p & 0xff00)<<8)
#define GET_BLUEER(p) ((p & 0xff)<<16)
#define GET_ALPHAER(p) ((p & 0xff0000)<<8)
/*These never change, DO NOT CHANGE THESE BASED ON COLOR INTERP BIT DEPTH*/
#define GET_RED(p) ((p>>16)&0xff)
#define GET_GREEN(p) ((p>>8)&0xff)
#define GET_BLUE(p) (p&0xff)
#define GET_ALPHA(p) ((p>>24)&0xff)
typedef GLuint PIXEL;
#define PSZB 4
#define PSZSH 5

#elif TGL_FEATURE_RENDER_BITS == 16

/* 16 bit mode */
#define GET_REDDER(p) ((p & 0xF800)<<8)
#define GET_GREENER(p) ((p & 0x07E0)<<13)
#define GET_BLUEER(p) ((p & 31)<<19)
#define GET_ALPHAER(p) (0)
#define GET_ALPHAMAX(p,a) (0)
/*DO NOT CHANGE THESE BASED ON COLOR INTERP BITDEPTH*/
#define GET_RED(p) ((p & 0xF800)>>8)
#define GET_GREEN(p) ((p & 0x07E0)>>3)
#define GET_BLUE(p) ((p & 31)<<3)
#define GET_ALPHA(p) (0)


typedef GLushort PIXEL;
#define PSZB 2
#define PSZSH 4


#else
#error "wrong TGL_FEATURE_RENDER_BITS"
#endif

#if TGL_FEATURE_LIT_TEXTURES == 1
#define RGB_MIX_FUNC(rr, gg, bb, aa, tpix) \
	RGB_TO_PIXEL( \
		((rr * GET_RED(tpix))>>8),\
		((gg * GET_GREEN(tpix))>>8),\
		((bb * GET_BLUE(tpix))>>8),\
		((aa * GET_ALPHA(tpix))>>8)\
	)
#else
#define RGB_MIX_FUNC(rr, gg, bb, aa, tpix)(tpix)
#endif



#define TGL_NO_BLEND_FUNC(source, dest){ZB_set(&(dest), source);}
#define TGL_NO_BLEND_FUNC_RGB(rr, gg, bb, aa, dest){ZB_set(&(dest), RGB_TO_PIXEL(rr,gg,bb,aa));}

#if TGL_FEATURE_BLEND == 1
#define TGL_BLEND_VARS GLuint zbblendeq = zb->blendeq; GLuint sfactor = zb->sfactor; GLuint dfactor = zb->dfactor;

/*SORCERY to achieve 32 bit signed integer clamping*/


#define TGL_BLEND_SWITCH_CASE(sr,sg,sb,sa,dr,dg,db,da,dest) 							\
		switch(zbblendeq){																\
			case GL_FUNC_ADD:															\
			default:																	\
				sr+=dr;sg+=dg;sb+=db;sa+=da;											\
				sr = TGL_CLAMPI(sr);													\
				sg = TGL_CLAMPI(sg);													\
				sb = TGL_CLAMPI(sb);													\
				sa = TGL_CLAMPI(sa);													\
				ZB_set(&(dest), RGB_TO_PIXEL(sr,sg,sb,sa));								\
			break;																		\
			case GL_FUNC_SUBTRACT:														\
				sr-=dr;sg-=dg;sb-=db;sa-=da;											\
				sr = TGL_CLAMPI(sr);													\
				sg = TGL_CLAMPI(sg);													\
				sb = TGL_CLAMPI(sb);													\
				sa = TGL_CLAMPI(sa);													\
				ZB_set(&(dest), RGB_TO_PIXEL(sr,sg,sb,sa));								\
			break;																		\
			case GL_FUNC_REVERSE_SUBTRACT:												\
				sr=dr-sr;sg=dg-sg;sb=db-sb;sa=da-sa;									\
				sr = TGL_CLAMPI(sr);													\
				sg = TGL_CLAMPI(sg);													\
				sb = TGL_CLAMPI(sb);													\
				sa = TGL_CLAMPI(sa);													\
				ZB_set(&(dest), RGB_TO_PIXEL(sr,sg,sb,sa));								\
			break;																		\
			case GL_FUNC_CHROMA_KEY:													\
				if (!(sr == 524287 && sg == 1114111 && sb == 720895)) {			       	\
					ZB_set(&(dest), RGB_TO_PIXEL(~sr,~sg,~sb,sa));                      \
				} 	                                                                    \
			break;																		\
		}																				\




#define TGL_BLEND_FUNC(source, dest){													\
	{																					\
	GLuint sr, sg, sb, sa, dr, dg, db, da;														\
	{	GLuint temp = source;																\
	sr = GET_REDDER(temp); sg = GET_GREENER(temp); sb = GET_BLUEER(temp); sa = GET_ALPHAER(temp);	\
	temp = dest;																			\
	dr = GET_REDDER(temp); dg = GET_GREENER(temp); db = GET_BLUEER(temp); da = GET_ALPHAER(temp);}	\
		/*printf("\nShould never reach this point!");*/									\
		switch(sfactor){															\
			case GL_ONE:																\
			default:																	\
			break;																		\
			case GL_ONE_MINUS_SRC_COLOR:												\
			sr = ~sr & COLOR_MASK;															\
			sg = ~sg & COLOR_MASK;															\
			sb = ~sb & COLOR_MASK;															\
			sa = ~sa & COLOR_MASK;															\
			break;																		\
			case GL_ZERO:																\
			sr=0;sg=0;sb=0;sa=0;break;													\
			break;																		\
		}																				\
		switch(dfactor){															\
				case GL_ONE:															\
				default:																\
				break;																	\
				case GL_ONE_MINUS_DST_COLOR:											\
				dr = ~dr & COLOR_MASK;														\
				dg = ~dg & COLOR_MASK;														\
				db = ~db & COLOR_MASK;														\
				da = ~da & COLOR_MASK;														\
				break;																	\
				case GL_ZERO:															\
				dr=0;dg=0;db=0;da=0;break;												\
				break;																	\
			}																			\
		TGL_BLEND_SWITCH_CASE(sr,sg,sb,sa,dr,dg,db,da,dest)								\
	}																					\
}

#define TGL_BLEND_FUNC_RGB(rr, gg, bb, aa, dest){											\
	{																					\
		GLint sr = rr & COLOR_MASK, sg = gg & COLOR_MASK, sb = bb & COLOR_MASK, sa = aa & COLOR_MASK, dr, dg, db, da;	\
		{GLuint temp = dest;																\
		dr = GET_REDDER(temp); dg = GET_GREENER(temp); db = GET_BLUEER(temp); da = GET_ALPHAER(temp);}					\
	/*printf("\nShould never reach this point!");*/										\
		switch(sfactor){																\
			case GL_ONE:																\
			default:																	\
			break;																		\
			case GL_ONE_MINUS_SRC_COLOR:												\
			sr = ~sr & COLOR_MASK;															\
			sg = ~sg & COLOR_MASK;															\
			sb = ~sb & COLOR_MASK;															\
			sa = ~sa & COLOR_MASK;															\
			break;																		\
			case GL_ZERO:																\
			sr=0;sg=0;sb=0;sa=0;break;													\
			break;																		\
			case GL_SRC_ALPHA:                                                          \
			break;                                                \
		}																				\
		switch(dfactor){															\
				case GL_ONE:															\
				default:																\
				break;																	\
				case GL_ONE_MINUS_DST_COLOR:											\
				dr = ~dr & COLOR_MASK;														\
				dg = ~dg & COLOR_MASK;														\
				db = ~db & COLOR_MASK;														\
				da = ~da & COLOR_MASK;														\
				break;																	\
				case GL_ZERO:															\
				dr=0;dg=0;db=0;da=0;break;												\
				break;																	\
				case GL_ONE_MINUS_SRC_ALPHA:                                               \
				break;                                                \
		}																				\
		TGL_BLEND_SWITCH_CASE(sr,sg,sb,sa,dr,dg,db,da,dest)								\
	}																					\
}

#else
#define TGL_BLEND_VARS /* a comment */
#define TGL_BLEND_FUNC(source, dest){ZB_set(&(dest), source);}
#define TGL_BLEND_FUNC_RGB(rr, gg, bb, aa, dest){ZB_set(&(dest), RGB_TO_PIXEL(rr,gg,bb,aa));}
#endif


typedef struct {



    GLushort *zbuf;
    PIXEL *pbuf;
    PIXEL *current_texture;


	/* point size*/
    GLfloat pointsize;





    /* opengl polygon stipple*/

#if TGL_FEATURE_POLYGON_STIPPLE == 1
    GLubyte stipplepattern[TGL_POLYGON_STIPPLE_BYTES];
    GLuint dostipple;
#endif
	GLenum blendeq, sfactor, dfactor;
    GLint enable_blend;
    GLint xsize,ysize;
    GLint linesize; /* line size, in bytes */
    GLint maxy;
    /* depth */
    GLint depth_test;
    GLint depth_write;
    GLubyte frame_buffer_allocated;
} ZBuffer;

typedef struct {
  GLint x,y,z;     /* integer coordinates in the zbuffer */
  GLint s,t;       /* coordinates for the mapping */
  GLint r,g,b,a;   /* color indexes */

  GLfloat sz,tz;   /* temporary coordinates for mapping */
} ZBufferPoint;

/* zbuffer.c */

ZBuffer *ZB_open(int xsize,int ysize,int mode, void *frame_buffer);
void ZB_close(ZBuffer *zb);
void ZB_resize(ZBuffer *zb,void *frame_buffer,GLint xsize,GLint ysize);
void ZB_clear(ZBuffer *zb,GLint clear_z,GLint z,
	      GLint clear_color,GLint r,GLint g,GLint b, GLint a);
void ZB_set(PIXEL *dst, PIXEL src);
/* linesize is in BYTES */
void ZB_copyFrameBuffer(ZBuffer *zb,void *buf,GLint linesize);

/* zdither.c */

/*
void ZB_initDither(ZBuffer *zb,GLint nb_colors,
		   unsigned char *color_indexes,GLint *color_table);
void ZB_closeDither(ZBuffer *zb);
void ZB_ditherFrameBuffer(ZBuffer *zb,unsigned char *dest,
			  GLint linesize);
*/
/* zline.c */

void ZB_plot(ZBuffer *zb,ZBufferPoint *p);
void ZB_line(ZBuffer *zb,ZBufferPoint *p1,ZBufferPoint *p2);
void ZB_line_z(ZBuffer * zb, ZBufferPoint * p1, ZBufferPoint * p2);

/* ztriangle.c */

void ZB_setTexture(ZBuffer *zb, PIXEL *texture);

void ZB_fillTriangleFlat(ZBuffer *zb,
		 ZBufferPoint *p1,ZBufferPoint *p2,ZBufferPoint *p3);

void ZB_fillTriangleFlatNOBLEND(ZBuffer *zb,
		 ZBufferPoint *p1,ZBufferPoint *p2,ZBufferPoint *p3);


void ZB_fillTriangleSmooth(ZBuffer *zb,
		   ZBufferPoint *p1,ZBufferPoint *p2,ZBufferPoint *p3);

void ZB_fillTriangleSmoothNOBLEND(ZBuffer *zb,
		   ZBufferPoint *p1,ZBufferPoint *p2,ZBufferPoint *p3);
/*
This function goes unused and is removed by Gek.
void ZB_fillTriangleMapping(ZBuffer *zb,
		    ZBufferPoint *p1,ZBufferPoint *p2,ZBufferPoint *p3);
*/
void ZB_fillTriangleMappingPerspective(ZBuffer *zb,
                    ZBufferPoint *p0,ZBufferPoint *p1,ZBufferPoint *p2);


void ZB_fillTriangleMappingPerspectiveNOBLEND(ZBuffer *zb,
                    ZBufferPoint *p0,ZBufferPoint *p1,ZBufferPoint *p2);

typedef void (*ZB_fillTriangleFunc)(ZBuffer  *,
	    ZBufferPoint *,ZBufferPoint *,ZBufferPoint *);

#endif /* _tgl_zbuffer_h_ */
