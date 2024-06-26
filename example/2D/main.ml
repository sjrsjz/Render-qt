#include<"include.ml">;

Shader_Main{
    N:buf0;
    N:buf1;
    N:buf0_s;
    R:time;
    N:SizeW;N:SizeH;
    N:Frame;
    N:bufA;N:bufB;
    N:buf2;
    Boolen:front;
    N:GroupSize;
    vec4:RGB[2];
    Traced cdecl var_update(N:name)->N:={
        front = true;
        if(
            CmpStr(name, "System.Renderer.Mouse.PosOnTexture")
           or CmpStr(name, "System.Renderer.Mouse.Position")
           ){
            return(1)
        };
        Frame = 0;
    }
    Traced cdecl frame_start()->N:={
        RenderSystemInterface:rs.Program = buf0_s;
        RGB[0] = rs.getVec4("RGB");
        RGB[1] = rs.getVec4("RGB2");

        front = not front;
        bufA = buf0;
        bufB = buf1;
        if(front){
            bufA = buf1;
            bufB = buf0
        };
    }
    Traced cdecl frame_update()->N:={
        RenderSystemInterface:rs.Program = buf0_s;
        rs.Shader(buf0_s);
        rs.Buf(bufA, 0);
        rs.Buf(bufB, 1);
        rs.Buf(buf2, 2);
        texture:texA = rs.getTex("MyTex");
        rs.Tex(texA.ID, 0, texA.dim);
        rs.setI("iW", SizeW);
        rs.setI("iH", SizeH);
        rs.setF4("RGBA", RGB[0]);
        rs.setF4("RGBA2", RGB[1]);
        rs.setF4("XY2D", rs.getVec4("System.Renderer.Camera.XY2D"));
	    rs.setF("iTime", rs.getFloat("System.Renderer.Time"));
        rs.setF("iScale", rs.getFloat("System.Renderer.Scale"));

        rs.setF4("XY2D_tex", rs.getVec4("System.Renderer.Mouse.PosOnTexture"));
        rs.setF4("iMouse", rs.getVec4("System.Renderer.Mouse.Button"));
        rs.setI("iFrame", Frame);

        rs.compute(SizeW, SizeH, 1, GroupSize, GroupSize, GroupSize);
        rs.Shader(0);
        return(1);//if zero,then keep updating
    }
    Traced cdecl frame_end()->N:={
        Frame = Frame + 1;
        return(bufB);//return the texture/buffer which you want to display
    }
    Traced cdecl shader_start()->N:={
        print("Hello Render-qt Version!\n");
        Frame = 0;
        front = true;
        RenderSystemInterface:rs;
        SizeW = rs.getI("System.Renderer.Width");
        SizeH = rs.getI("System.Renderer.Height");
        GroupSize = rs.getI("System.Renderer.WorkGroupSize");
        buf0 = rs.buffer2D(SizeW, SizeH);
        buf1 = rs.buffer2D(SizeW, SizeH);
        buf2 = rs.buffer2D(SizeW, SizeH);
        buf0_s = rs.getShader("buf0");
    }
    Traced cdecl shader_end()->N:={
        RenderSystemInterface:rs;
        rs.freeTex(buf0);
        rs.freeTex(buf1);
        rs.freeTex(buf2)
    }
}