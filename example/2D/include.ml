#include<"string.ml">;

N:DefaultSize;
N:DefaultPixelSize;
N:DefaultLineSize;
N:ifOutputBigImage;
N:ifOutputRAW;

Main{
    Traced main()->N:={
        Framework_Main()
    }
}
_vector_{
    Traced vec4(R:x,R:y,R:z,R:w)->vec4:={
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
    Traced _init_()->N:={x=0;y=0;z=0;w=0}
    Traced _destroy_()->N:={}
    Traced Public +(vec4:o)->vec4:={
        vec4:t;
        t.x=x+o.x;t.y=y+o.y;t.z=z+o.z;t.w=w+o.w;
        return(t);
    }
    Traced Public -(vec4:o)->vec4:={
        vec4:t;
        t.x=x-o.x;t.y=y-o.y;t.z=z-o.z;t.w=w-o.w;
        return(t);
    }
    Traced Public *(vec4:o)->vec4:={
        vec4:t;
        t.x=x*o.x;t.y=y*o.y;t.z=z*o.z;t.w=w*o.w;
        return(t);
    }
    Traced Public scale(R:o)->vec4:={
        vec4:t;
        t.x=x*o;t.y=y*o;t.z=z*o;t.w=w*o;
        return(t);
    }
}

Class:RenderSystemInterface{
    N:_this;
    _RenderSystem_:sys;
    Public N:Program;
    Traced _init_()->N:={
        _this=sys._this_;
        Program=0;
    }
    Traced Public buffer1D(N:size)->N:={
        return(image1D(_this,size))
    }
    Traced Public buffer2D(N:w,N:h)->N:={
        return(image2D(_this,w,h))
    }
    Traced Public buffer3D(N:l,N:w,N:h)->N:={
        return(image3D(_this,l,w,h))
    }
    Traced Public Tex(N:tex,N:bind,N:dim)->N:={
        return(bindTex(_this,tex,bind,dim))
    }
    Traced Public Buf(N:tex,N:unit)->N:={
        return(bindImage(_this,tex,unit))
    }
    Traced Public freeTex(N:tex)->N:={
        return(releaseImage(_this,tex))
    }
    Traced Public Shader(N:shader)->N:={
        return(bindProgram(_this,shader))
    }
    Traced Public getShader(N:str)->N:={
        return(getShader_(_this,str))
    }
    Traced Public compute(N:l,N:w,N:h,N:g_l,N:g_w,N:g_h)->N:={
        return(compute(_this,Program,l,w,h,g_l,g_w,g_h))
    }
    Traced Public setVar(N:str)->N:={
        return(setVar(_this,Program,str))
    }
    Traced Public setVars()->N:={
        return(setVars(_this,Program))
    }
    Traced Public setF(N:str,R:x)->N:={
        return(uniform1f(_this,Program,str,x))
    }
    Traced Public setF3(N:str,R:x,R:y,R:z)->N:={
        return(uniform3f(_this,Program,str,x,y,z))
    }
    Traced Public setF4(N:str,R:x,R:y,R:z,R:w)->N:={
        return(uniform4f(_this,Program,str,x,y,z,w))
    }
    Traced Public setF4(N:str,vec4:v)->N:={
        return(uniform4f(_this,Program,str,v.x,v.y,v.z,v.w))
    }
    Traced Public setI(N:str,N:x)->N:={
        return(uniform1i(_this,Program,str,x))
    }
    Traced Public setMat2x2(N:str,R:x[4])->N:={
        return(uniformMatrix2x2f(_this,Program,str,x))
    }
    Traced Public setMat3x3(N:str,R:x[9])->N:={
        return(uniformMatrix3x3f(_this,Program,str,x))
    }
    Traced Public setMat4x4(N:str,R:x[16])->N:={
        return(uniformMatrix4x4f(_this,Program,str,x))
    }
    Traced Public setEyeMat()->N:={
        return(setVar(_this,Program,"EyeMat"))
    }
    Traced Public setCallBack(N:name,N:func)->N:={
        return(sys.setCallback(name,func))
    }
    Traced Public getFloat(N:str)->R:={
        R:t;
        getUniform1f(_this, str, &t);
        return(t)
    }
    Traced Public getVec4(N:str)->vec4:={
        vec4:t;
        getUniform4f(_this, str, &t);
        return(t)
    }
    Traced Public getI(N:str)->Z:={
        return(getUniform1i(_this, str))
    }
    Traced Public CmdArg(N:name)->string:={
        B:buffer[2048];
        getCmdLineArg(_this,name,buffer);
        return(string(buffer))
    }
    Traced Public setVec4(N:str,vec4:vec)->N:={uniform4f(_this,Program,str,vec.x,vec.y,vec.z,vec.w)}
    Traced Public setVec3(N:str,vec4:vec)->N:={uniform3f(_this,Program,str,vec.x,vec.y,vec.z)}
    Traced Transit "" image1D(N:this_,N:size)->N:={return(sys.getFunction("image1D"))}
    Traced Transit "" image2D(N:this_,N:w,N:h)->N:={return(sys.getFunction("image2D"))}
    Traced Transit "" image3D(N:this_,N:l,N:w,N:h)->N:={return(sys.getFunction("image3D"))}
    Traced Transit "" bindImage(N:this_,N:id,N:unit)->N:={return(sys.getFunction("bindImage"))}
    Traced Transit "" bindTex(N:this_,N:id,N:bind,N:dim)->N:={return(sys.getFunction("bindTex"))}
    Traced Transit "" releaseImage(N:this_,N:id)->N:={return(sys.getFunction("releaseImage"))}
    Traced Transit "" bindProgram(N:this_,N:id)->N:={return(sys.getFunction("bindProgram"))}
    Traced Transit "" getShader_(N:this_,N:str)->N:={return(sys.getFunction("getShader"))}
    Traced Transit "" compute(N:this_,N:program,N:l,N:w,N:h,N:g_l,N:g_w,N:g_h)->N:={return(sys.getFunction("compute"))}
    Traced Transit "" setVar(N:this_,N:program,N:str)->N:={return(sys.getFunction("setVar"))}
    Traced Transit "" setVars(N:this_,N:program)->N:={return(sys.getFunction("setVars"))}
    Traced Transit "" uniform1f(N:this_,N:program,N:str,R:x)->N:={return(sys.getFunction("uniform1f"))}
    Traced Transit "" uniform2f(N:this_,N:program,N:str,R:x,R:y,R:z)->N:={return(sys.getFunction("uniform2f"))}
    Traced Transit "" uniform3f(N:this_,N:program,N:str,R:x)->N:={return(sys.getFunction("uniform3f"))}
    Traced Transit "" uniform4f(N:this_,N:program,N:str,R:x,R:y,R:z,R:w)->N:={return(sys.getFunction("uniform4f"))}
    Traced Transit "" uniform1i(N:this_,N:program,N:str,N:x)->N:={return(sys.getFunction("uniform1i"))}
    Traced Transit "" uniformMatrix2x2f(N:this_,N:program,N:str,R:x[4])->N:={return(sys.getFunction("uniformMatrix2x2f"))}
    Traced Transit "" uniformMatrix3x3f(N:this_,N:program,N:str,R:x[9])->N:={return(sys.getFunction("uniformMatrix3x3f"))}
    Traced Transit "" uniformMatrix4x4f(N:this_,N:program,N:str,R:x[16])->N:={return(sys.getFunction("uniformMatrix4x4f"))}
    Traced Transit "" getUniform1f(N:this_,N:str,N:v)->N:={return(sys.getFunction("getUniform1f"))}
    Traced Transit "" getUniform2f(N:this_,N:str,N:v)->N:={return(sys.getFunction("getUniform2f"))}
    Traced Transit "" getUniform3f(N:this_,N:str,N:v)->N:={return(sys.getFunction("getUniform3f"))}
    Traced Transit "" getUniform4f(N:this_,N:str,N:v)->N:={return(sys.getFunction("getUniform4f"))}
    Traced Transit "" getUniform1i(N:this_,N:str)->N:={return(sys.getFunction("getUniform1i"))}
    Traced Transit "" getUniformMatrix2x2f(N:this_,N:str,N:v)->N:={return(sys.getFunction("getUniformMatrix2x2f"))}
    Traced Transit "" getUniformMatrix3x3f(N:this_,N:str,N:v)->N:={return(sys.getFunction("getUniformMatrix3x3f"))}
    Traced Transit "" getUniformMatrix4x4f(N:this_,N:str,N:v)->N:={return(sys.getFunction("getUniformMatrix4x4f"))}
    Traced Transit "" getCmdLineArg(N:this_,N:str,N:v)->N:={return(sys.getFunction("getCmdLineArg"))}
}


Framework{
    Traced Framework_Main()->N := {
    RenderSystemInterface:rs;
    rs.setCallBack("frame_start", ~frame_start);
    rs.setCallBack("frame_update", ~frame_update);
    rs.setCallBack("frame_end", ~frame_end);
    rs.setCallBack("shader_start", ~shader_start);
    rs.setCallBack("shader_end", ~shader_end);
    rs.setCallBack("var_update", ~var_update);
    shader_start();
    }

}