/******************************************************************************/
#include "!Header.h"
/******************************************************************************
SKIN, ALPHA_TEST
/******************************************************************************/
void VS
(
   VtxInput vtx,

   out Vec4 outVtx:POSITION,
#if ALPHA_TEST
   out Vec2 outTex:TEXCOORD,
#endif
   out VecH outNrm:NORMAL, // !! not Normalized !!
   out Vec  outPos:POS
)
{
#if ALPHA_TEST
   outTex=vtx.tex();
#endif

   if(!SKIN)
   {
                     outNrm=TransformDir(vtx.nrm());
      outVtx=Project(outPos=TransformPos(vtx.pos()));
   }else
   {
      VecU bone=vtx.bone();
                     outNrm=TransformDir(vtx.nrm(), bone, vtx.weight());
      outVtx=Project(outPos=TransformPos(vtx.pos(), bone, vtx.weight()));
   }
}
/******************************************************************************/
VecH4 PS
(
   PIXEL,
#if ALPHA_TEST
   Vec2 inTex:TEXCOORD,
#endif
   VecH inNrm:NORMAL,
   Vec  inPos:POS
):TARGET
{
#if ALPHA_TEST
   MaterialAlphaTest(Tex(Col, inTex).a);
#endif

   Half alpha=Sat((Half(inPos.z-TexDepthPoint(PixelToUV(pixel)))-BehindBias)/0.3);

   VecH4  col   =Lerp(Color[0], Color[1], Abs(Normalize(inNrm).z));
          col.a*=alpha;
   return col;
}
/******************************************************************************/
