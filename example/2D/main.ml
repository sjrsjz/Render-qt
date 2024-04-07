#include<"include.ml">;

Shader_Main{
    N:buf0;
    N:buf1;
    N:buf0_s;
    R:time;
    N:SizeW;N:SizeH;
    N:Frame;
    N:bufA;N:bufB;N:buf2;
    Boolen:front;N:Offset;
    N:lOffset;
    vec4:Position2D;
    vec4:RGB[2];
    R:scale;
    var_update(N:name)->N:={
        Frame=0;front=1
    }
    frame_start()->N:={
        RenderSystemInterface:rs;
        Offset = rs.getFloat(&"offset");
        scale = rs.getFloat(&"scale");
        Position2D = rs.getVec4(&"Position2D");
        RGB[0] = rs.getVec4(&"RGB");
        RGB[1] = rs.getVec4(&"RGB2");
        if(lOffset != Offset){
            Frame = 0
        };
        lOffset = Offset;
        front = not front;
        bufA = buf0;
        bufB = buf1;
        if(front){
            bufA = buf1;
            bufB = buf0
        };
        time = rs.getFloat("time")
    }
    frame_update()->N:={
        RenderSystemInterface:rs;
        rs.Shader(buf0_s);
        rs.Buf(bufA,0);
        rs.Buf(bufB, 1);
        rs.Buf(buf2, 2);
        rs.setF(&"iTime", time);
        rs.setI(&"iFrame", Frame);
        rs.setI(&"iOffset", Offset);
        rs.setI(&"iW", SizeW);
        rs.setI(&"iH", SizeH);
        rs.setF(&"scale", scale);
        rs.setVec4(&"Position2D", Position2D);
        rs.setVec3(&"RGB", RGB[0]);
        rs.setVec3(&"RGB2", RGB[1]);
        N:size0=SizeW/DefaultPixelSize;
        N:size1=SizeH/DefaultPixelSize;
        rs.compute(size0, size1, 1);
        rs.Shader(0);
        return(1);//if zero,then keep updating
    }
    frame_end()->N:={
        Frame=Frame+1;
        return(bufB);//return the texture/buffer which you want to display
    }
    shader_start()->N:={
        Frame=0;
        front=true;
        lOffset=0;
        SizeW=DefaultSize;SizeH=SizeW;
        if(ifOutputBigImage!=0){
            SizeH=DefaultLineSize*DefaultPixelSize;
        };
        RenderSystemInterface:rs;
        buf0 = rs.buffer2D(SizeW, SizeH);
        buf1 = rs.buffer2D(SizeW, SizeH);
        buf2 = rs.buffer2D(SizeW, SizeH);
        buf0_s = rs.getShader("buf0");
    }
    shader_end()->N:={
        RenderSystemInterface:rs;
        rs.freeTex(buf0);
        rs.freeTex(buf1);
        rs.freeTex(buf2)
    }
}