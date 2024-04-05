N:DefaultSize;
N:DefaultPixelSize;
N:DefaultLineSize;
N:ifOutputBigImage;
N:ifOutputRAW;

Main{
    main()->N:={
        Framework_Main()
    }
}
_vector_{
    vec4(R:x,R:y,R:z,R:w)->vec4:={
        vec4:t;
        t.x=x;t.y=y;t.z=z;t.w=w;
        return(t)
    }
}
Class:vec4{
    Public R:x;
    Public R:y;
    Public R:z;
    Public R:w;
    _init_()->N:={x=0;y=0;z=0;w=0}
    _destroy_()->N:={}
    Public +(vec4:o)->vec4:={
        vec4:t;
        t.x=x+o.x;t.y=y+o.y;t.z=z+o.z;t.w=w+o.w;
        return(t);
    }
    Public -(vec4:o)->vec4:={
        vec4:t;
        t.x=x-o.x;t.y=y-o.y;t.z=z-o.z;t.w=w-o.w;
        return(t);
    }
    Public *(vec4:o)->vec4:={
        vec4:t;
        t.x=x*o.x;t.y=y*o.y;t.z=z*o.z;t.w=w*o.w;
        return(t);
    }
    Public scale(R:o)->vec4:={
        vec4:t;
        t.x=x*o;t.y=y*o;t.z=z*o;t.w=w*o;
        return(t);
    }
}
Framework{
    N:buffer1D;
    N:buffer2D;
    N:buffer3D;
    N:Buf;
    N:compute;
    N:freeTex;
    N:Shader;
    N:getShader;
    N:setF;
    N:setF3;
    N:setI;
    N:setI3;
    N:setD;
    N:setD3;
    N:setF4;
    N:setEyeMat;
    N:setVar;
    N:setVars;
    N:getTime;
    N:getFloat;
    N:getVec4;
    N:LoadTex;
    N:Tex;
    N:getFFT;
    N:playFFT;
    N:BuildUpMash;
    N:TexMash;
    N:GetTexMash;
    N:UpdateMash;
    N:ClearMashData;
    N:PushMashData;
    N:TexLWH;
    _RenderSystem_:sys;
    Framework_Main()->N:={
        buffer1D=sys.getFunction("buffer1D");
        buffer2D=sys.getFunction("buffer2D");
        buffer3D=sys.getFunction("buffer3D");
        Buf=sys.getFunction("Buf");
        compute=sys.getFunction("compute");
        freeTex=sys.getFunction("freeTex");
        Shader=sys.getFunction("Shader");
        getShader=sys.getFunction("getShader");
        setF=sys.getFunction("setF");
        setF3=sys.getFunction("setF3");
        setF4=sys.getFunction("setF4");
        setI=sys.getFunction("setI");
        setI3=sys.getFunction("setI3");
        setD=sys.getFunction("setD");
        setD3=sys.getFunction("setD3");
        setVar=sys.getFunction("setVar");
        setEyeMat=sys.getFunction("setEyeMat");
        Tex=sys.getFunction("Tex");
        LoadTex=sys.getFunction("LoadTex");
        getFloat=sys.getFunction("getFloat");
        getVec4=sys.getFunction("getVec4");
        getTime=sys.getFunction("time");
        getFFT=sys.getFunction("getFFT");
        playFFT=sys.getFunction("playFFT");
        BuildUpMash=sys.getFunction("BuildUpMash");
        TexMash=sys.getFunction("TexMash");
        GetTexMash=sys.getFunction("GetTexMash");
        UpdateMash=sys.getFunction("UpdateMash");
        PushMashData=sys.getFunction("PushMashData");
        ClearMashData=sys.getFunction("ClearMashData");
        setVars=sys.getFunction("setVars");
        TexLWH=sys.getFunction("TexLWH");
        sys.setCallback("frame_start",~frame_start);
        sys.setCallback("frame_end",~frame_end);
        sys.setCallback("frame_update",~frame_update);
        sys.setCallback("shader_end",~shader_end);
        sys.setCallback("var_update",~var_update);

        R:t;
        getFloat(&"DefaultSize",&t);DefaultSize=t;
        getFloat(&"DefaultPixelSize",&t);DefaultPixelSize=t;
        getFloat(&"DefaultLineSize",&t);DefaultLineSize=t;
        getFloat(&"ifOutputBigImage",&t);ifOutputBigImage=t;
        getFloat(&"ifOutputRAW",&t);ifOutputRAW=t;
        shader_start();
    }
    setVec4(N:str,vec4:vec)->N:={setF4(str,vec.x,vec.y,vec.z,vec.w)}
    setVec3(N:str,vec4:vec)->N:={setF3(str,vec.x,vec.y,vec.z)}
    Transit "" buffer1D(N:size)->N:={return(buffer1D)}
    Transit "" buffer2D(N:w,N:h)->N:={return(buffer2D)}
    Transit "" buffer3D(N:l,N:w,N:h)->N:={return(buffer3D)}
    Transit "" Buf(N:id,N:unit)->N:={return(Buf)}
    Transit "" freeTex(N:id)->N:={return(freeTex)}
    Transit "" Shader(N:id)->N:={return(Shader)}
    Transit "" getShader(N:str)->N:={return(getShader)}
    Transit "" compute(N:l,N:w,N:h)->N:={return(compute)}
    Transit "" setVar(N:str)->N:={return(setVar)}
    Transit "" setVars()->N:={return(setVars)}
    Transit "" setF(N:str,R:x)->N:={return(setF)}
    Transit "" setF3(N:str,R:x,R:y,R:z)->N:={return(setF3)}
    Transit "" setD(N:str,R:x)->N:={return(setD)}
    Transit "" setD3(N:str,R:x,R:y,R:z)->N:={return(setD3)}
    Transit "" setI(N:str,N:x)->N:={return(setI)}
    Transit "" setI3(N:str,N:x,N:y,N:z)->N:={return(setI3)}
    Transit "" setF4(N:str,R:x,R:y,R:z,R:w)->N:={return(setF4)}
    Transit "" time(N:t)->N:={return(getTime)}
    Transit "" setEyeMat(N:str)->N:={return(setEyeMat)}
    Transit "" Tex(N:id,N:bind,N:dim)->N:={return(Tex)}
    Transit "" LoadTex(N:str)->N:={return(LoadTex)}
    Transit "" playFFT(N:tex,N:stream,N:length,N:start,N:playlength)->N:={return(playFFT)}
    Transit "" getFloat(N:str,N:t)->N:={return(getFloat)}
    Transit "" getVec4(N:str,vec4:t)->N:={return(getVec4)}
    Transit "" getFFT(N:str,N:tex,N:size)->N:={return(getFFT)}
    Transit "" BuildUpMash(N:tex)->N:={return(BuildUpMash)}
    Transit "" TexMash(N:/*&*/tex)->N:={return(TexMash)}
    Transit "" GetTexMash(N:tex)->N:={return(GetTexMash)}
    Transit "" UpdateMash()->N:={return(UpdateMash)}
    Transit "" PushMashData()->N:={return(PushMashData)}
    Transit "" ClearMashData()->N:={return(ClearMashData)}
    Transit "" TexLWH(N:tex,vec4:dim)->N:={return(TexLWH)}
    GetFloat(N:str)->R:={R:t;getFloat(str,&t);return(t)}
}