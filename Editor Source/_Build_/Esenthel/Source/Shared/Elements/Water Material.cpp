﻿/******************************************************************************/
#include "stdafx.h"
/******************************************************************************/

/******************************************************************************/
   EditWaterMtrl::EditWaterMtrl() : density(0.3f), density_add(0.45f), scale_color(1.0f/200), scale_normal(1.0f/10), scale_bump(1.0f/100), refract(0.10f), refract_reflection(0.06f), refract_underwater(0.01f), wave_scale(0.25f), color_underwater0(0.26f, 0.35f, 0.42f), color_underwater1(0.10f, 0.20f, 0.30f) {smooth=1; reflect_min=0.02f;}
   bool EditWaterMtrl::usesTexBump()C {return wave_scale>EPSL && hasBumpMap();}
   bool EditWaterMtrl::equal(C EditWaterMtrl &src)C
   {
      return super::equal(src)
         && density_time==src.density_time
         && scale_color_time==src.scale_color_time && scale_normal_time==src.scale_normal_time && scale_bump_time==src.scale_bump_time
         && refract_time==src.refract_time && refract_reflection_time==src.refract_reflection_time && refract_underwater_time==src.refract_underwater_time
         && wave_scale_time==src.wave_scale_time
         && color_underwater_time==src.color_underwater_time;
   }
   bool EditWaterMtrl::newer(C EditWaterMtrl &src)C
   {
      return super::newer(src)
         || density_time>src.density_time
         || scale_color_time>src.scale_color_time || scale_normal_time>src.scale_normal_time || scale_bump_time>src.scale_bump_time
         || refract_time>src.refract_time || refract_reflection_time>src.refract_reflection_time || refract_underwater_time>src.refract_underwater_time
         || wave_scale_time>src.wave_scale_time
         || color_underwater_time>src.color_underwater_time;
   }
   void EditWaterMtrl::reset() {T=EditWaterMtrl();}
   void EditWaterMtrl::newData()
   {
      super::newData();
      density_time++;
      scale_color_time++; scale_normal_time++; scale_bump_time++;
      refract_time++; refract_reflection_time++; refract_underwater_time++;
      wave_scale_time++;
      color_underwater_time++;
   }
   void EditWaterMtrl::create(C WaterMtrl &src, C TimeStamp &time)
   {
      super::create(Material(), time); // call super to setup times for all values
      color_s               =Vec4(src.colorS(), 1); color_time=time;
      smooth                =src.smooth ; smooth_time=time;
      reflect_min           =src.reflect; reflect_time=time;
      normal                =src.normal ; normal_time=time;
      wave_scale            =src.wave_scale; wave_scale_time=time;
      scale_color           =src.scale_color; scale_color_time=time;
      scale_normal          =src.scale_normal; scale_normal_time=time;
      scale_bump            =src.scale_bump; scale_bump_time=time;
      density               =src.density;
      density_add           =src.density_add; density_time=time;
      refract               =src.refract; refract_time=time;
      refract_reflection    =src.refract_reflection; refract_reflection_time=time;
      refract_underwater    =src.refract_underwater; refract_underwater_time=time;
      color_underwater0     =src.colorUnderwater0S(); color_underwater_time=time;
      color_underwater1     =src.colorUnderwater1S();
      base_0_tex=src. colorMap().id();
      base_1_tex=src.normalMap().id();
      base_2_tex=src.  bumpMap().id();
   }
   void EditWaterMtrl::copyTo(WaterMtrl &dest, C Project &proj)C
   {
      dest.colorS(color_s.xyz);
      dest.smooth                =smooth            ;
      dest.reflect               =reflect_min       ;
      dest.normal                =normal            ;
      dest.wave_scale            =wave_scale        ;
      dest.scale_color           =scale_color       ;
      dest.scale_normal          =scale_normal      ;
      dest.scale_bump            =scale_bump        ;
      dest.density               =density           ;
      dest.density_add           =density_add       ;
      dest.refract               =refract           ;
      dest.refract_reflection    =refract_reflection;
      dest.refract_underwater    =refract_underwater;
      dest.colorUnderwater0S     (color_underwater0);
      dest.colorUnderwater1S     (color_underwater1);
      dest. colorMap(proj.texPath(base_0_tex))
          .normalMap(proj.texPath(base_1_tex))
          .  bumpMap(proj.texPath(base_2_tex));
      dest.validate();
   }
   uint EditWaterMtrl::sync(C EditMaterial  &src) {return super::sync(src);}
   uint EditWaterMtrl::sync(C EditWaterMtrl &src)
   {
      uint changed=super::sync(src);
      changed|=Sync(wave_scale_time        , src.wave_scale_time        , wave_scale        , src.wave_scale        )*CHANGED_PARAM;
      changed|=Sync(scale_color_time       , src.scale_color_time       , scale_color       , src.scale_color       )*CHANGED_PARAM;
      changed|=Sync(scale_normal_time      , src.scale_normal_time      , scale_normal      , src.scale_normal      )*CHANGED_PARAM;
      changed|=Sync(scale_bump_time        , src.scale_bump_time        , scale_bump        , src.scale_bump        )*CHANGED_PARAM;
      changed|=Sync(refract_time           , src.refract_time           , refract           , src.refract           )*CHANGED_PARAM;
      changed|=Sync(refract_reflection_time, src.refract_reflection_time, refract_reflection, src.refract_reflection)*CHANGED_PARAM;
      changed|=Sync(refract_underwater_time, src.refract_underwater_time, refract_underwater, src.refract_underwater)*CHANGED_PARAM;
      if(Sync(density_time, src.density_time))
      {
         changed|=CHANGED_PARAM;
         density    =src.density;
         density_add=src.density_add;
      }
      if(Sync(color_underwater_time, src.color_underwater_time))
      {
         changed|=CHANGED_PARAM;
         color_underwater0=src.color_underwater0;
         color_underwater1=src.color_underwater1;
      }
      return changed;
   }
   uint EditWaterMtrl::undo(C EditWaterMtrl &src)
   {
      uint changed=super::undo(src);
      changed|=Undo(wave_scale_time        , src.wave_scale_time        , wave_scale        , src.wave_scale        )*CHANGED_PARAM;
      changed|=Undo(scale_color_time       , src.scale_color_time       , scale_color       , src.scale_color       )*CHANGED_PARAM;
      changed|=Undo(scale_normal_time      , src.scale_normal_time      , scale_normal      , src.scale_normal      )*CHANGED_PARAM;
      changed|=Undo(scale_bump_time        , src.scale_bump_time        , scale_bump        , src.scale_bump        )*CHANGED_PARAM;
      changed|=Undo(refract_time           , src.refract_time           , refract           , src.refract           )*CHANGED_PARAM;
      changed|=Undo(refract_reflection_time, src.refract_reflection_time, refract_reflection, src.refract_reflection)*CHANGED_PARAM;
      changed|=Undo(refract_underwater_time, src.refract_underwater_time, refract_underwater, src.refract_underwater)*CHANGED_PARAM;
      if(Undo(density_time, src.density_time))
      {
         changed|=CHANGED_PARAM;
         density    =src.density;
         density_add=src.density_add;
      }
      if(Undo(color_underwater_time, src.color_underwater_time))
      {
         changed|=CHANGED_PARAM;
         color_underwater0=src.color_underwater0;
         color_underwater1=src.color_underwater1;
      }
      return changed;
   }
   bool EditWaterMtrl::save(File &f)C
   {
      f.cmpUIntV(2); // version
      super::save(f);
      f<<density<<density_add
       <<scale_color<<scale_normal<<scale_bump
       <<refract<<refract_reflection<<refract_underwater
       <<wave_scale
       <<color_underwater0
       <<color_underwater1
       <<density_time
       <<scale_color_time<<scale_normal_time<<scale_bump_time
       <<refract_time<<refract_reflection_time<<refract_underwater_time
       <<wave_scale_time
       <<color_underwater_time;
      return f.ok();
   }
   bool EditWaterMtrl::load(File &f)
   {
      switch(f.decUIntV()) // version
      {
         case 2: if(super::load(f))
         {
            f>>density>>density_add
             >>scale_color>>scale_normal>>scale_bump
             >>refract>>refract_reflection>>refract_underwater
             >>wave_scale
             >>color_underwater0
             >>color_underwater1
             >>density_time
             >>scale_color_time>>scale_normal_time>>scale_bump_time
             >>refract_time>>refract_reflection_time>>refract_underwater_time
             >>wave_scale_time
             >>color_underwater_time;
            if(f.ok())return true;
         }break;

         case 1: if(super::load(f))
         {
            flt temp_flt; Vec temp_vec; TimeStamp temp_time;
            f>>density>>density_add>>temp_flt>>temp_flt>>scale_color>>scale_normal>>scale_bump
             >>temp_flt>>refract>>refract_reflection>>refract_underwater>>wave_scale
             >>temp_flt>>temp_flt>>temp_vec>>color_underwater0>>color_underwater1
             >>density_time>>temp_time>>scale_color_time>>scale_normal_time>>scale_bump_time
             >>temp_time>>refract_time>>refract_reflection_time>>refract_underwater_time>>wave_scale_time
             >>temp_time>>temp_time>>temp_time>>color_underwater_time;
            scale_color =1/scale_color ;
            scale_normal=1/scale_normal;
            scale_bump  =1/scale_bump  ;
            if(f.ok())return true;
         }break;

         case 0: reset(); return f.ok(); // empty
      }
      reset(); return false;
   }
   bool EditWaterMtrl::load(C Str &name)
   {
      File f; if(f.readTry(name))return load(f);
      reset(); return false;
   }
/******************************************************************************/
