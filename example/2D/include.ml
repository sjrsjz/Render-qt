N:DefaultSize;
N:DefaultPixelSize;
N:DefaultLineSize;
N:ifOutputBigImage;
N:ifOutputRAW;

Main{
    main()->N:={
        print("Hello World\n");
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

Class:RenderSystemInterface{
    N:_this;
    _RenderSystem_:sys;
    _init_()->N:={
        _this=sys._this_;
    }
    Public buffer1D(N:size)->N:={
        return(image1D(_this,size))
    }
    Public buffer2D(N:w,N:h)->N:={
        return(image2D(_this,w,h))
    }
    Public buffer3D(N:l,N:w,N:h)->N:={
        return(image3D(_this,l,w,h))
    }
    Public Tex(N:tex,N:bind,N:dim)->N:={
        return(bindTex(_this,tex,bind,dim))
    }
    Public Buf(N:tex,N:unit)->N:={
        return(bindImage(_this,tex,unit))
    }
    Public freeTex(N:tex)->N:={
        return(releaseImage(_this,tex))
    }
    Public Shader(N:shader)->N:={
        return(bindProgram(_this,shader))
    }
    Public getShader(N:str)->N:={
        return(getShader_(_this,str))
    }
    Public compute(N:l,N:w,N:h)->N:={
        return(compute(_this,l,w,h))
    }
    Public setVar(N:str)->N:={
        return(setVar(_this,str))
    }
    Public setVars()->N:={
        return(setVars(_this))
    }
    Public setF(N:str,R:x)->N:={
        return(uniform1f(_this,str,x))
    }
    Public setF3(N:str,R:x,R:y,R:z)->N:={
        return(uniform3f(_this,str,x,y,z))
    }
    Public setF4(N:str,R:x,R:y,R:z,R:w)->N:={
        return(uniform4f(_this,str,x,y,z,w))
    }
    Public setI(N:str,N:x)->N:={
        return(uniform1i(_this,str,x))
    }
    Public setMat2x2(N:str,R:x[4])->N:={
        return(uniformMatrix2x2f(_this,str,x))
    }
    Public setMat3x3(N:str,R:x[9])->N:={
        return(uniformMatrix3x3f(_this,str,x))
    }
    Public setMat4x4(N:str,R:x[16])->N:={
        return(uniformMatrix4x4f(_this,str,x))
    }
    Public setEyeMat()->N:={
        return(setVar(_this,"EyeMat"))
    }
    Public setCallBack(N:name,N:func)->N:={
        return(sys.setCallback(name,func))
    }
    Public getFloat(N:str)->R:={
        R:t;
        getUniform1f(_this, str, &t);
        return(t)
    }
    Public getVec4(N:str)->vec4:={
        vec4:t;
        getUniform4f(_this, str, &t);
        return(t)
    }

    Public setVec4(N:str,vec4:vec)->N:={uniform4f(_this,str,vec.x,vec.y,vec.z,vec.w)}
    Public setVec3(N:str,vec4:vec)->N:={uniform3f(_this,str,vec.x,vec.y,vec.z)}
    Transit "" image1D(N:this_,N:size)->N:={return(sys.getFunction("image1D"))}
    Transit "" image2D(N:this_,N:w,N:h)->N:={return(sys.getFunction("image2D"))}
    Transit "" image3D(N:this_,N:l,N:w,N:h)->N:={return(sys.getFunction("image3D"))}
    Transit "" bindImage(N:this_,N:id,N:unit)->N:={return(sys.getFunction("bindImage"))}
    Transit "" bindTex(N:this_,N:id,N:bind,N:dim)->N:={return(sys.getFunction("bindTex"))}
    Transit "" releaseImage(N:this_,N:id)->N:={return(sys.getFunction("releaseImage"))}
    Transit "" bindProgram(N:this_,N:id)->N:={return(sys.getFunction("bindProgram"))}
    Transit "" getShader_(N:this_,N:str)->N:={return(sys.getFunction("getShader"))}
    Transit "" compute(N:this_,N:l,N:w,N:h)->N:={return(sys.getFunction("compute"))}
    Transit "" setVar(N:this_,N:str)->N:={return(sys.getFunction("setVar"))}
    Transit "" setVars(N:this_,)->N:={return(sys.getFunction("setVars"))}
    Transit "" uniform1f(N:this_,N:str,R:x)->N:={return(sys.getFunction("uniform1f"))}
    Transit "" uniform2f(N:this_,N:str,R:x,R:y,R:z)->N:={return(sys.getFunction("uniform2f"))}
    Transit "" uniform3f(N:this_,N:str,R:x)->N:={return(sys.getFunction("uniform3f"))}
    Transit "" uniform4f(N:this_,N:str,R:x,R:y,R:z,R:w)->N:={return(sys.getFunction("uniform4f"))}
    Transit "" uniform1i(N:this_,N:str,N:x)->N:={return(sys.getFunction("uniform1i"))}
    Transit "" uniformMatrix2x2f(N:this_,N:str,R:x[4])->N:={return(sys.getFunction("uniformMatrix2x2f"))}
    Transit "" uniformMatrix3x3f(N:this_,N:str,R:x[9])->N:={return(sys.getFunction("uniformMatrix3x3f"))}
    Transit "" uniformMatrix4x4f(N:this_,N:str,R:x[16])->N:={return(sys.getFunction("uniformMatrix4x4f"))}
    Transit "" getUniform1f(N:this_,N:str,N:v)->N:={return(sys.getFunction("getUniform1f"))}
    Transit "" getUniform2f(N:this_,N:str,N:v)->N:={return(sys.getFunction("getUniform2f"))}
    Transit "" getUniform3f(N:this_,N:str,N:v)->N:={return(sys.getFunction("getUniform3f"))}
    Transit "" getUniform4f(N:this_,N:str,N:v)->N:={return(sys.getFunction("getUniform4f"))}
    Transit "" getUniform1i(N:this_,N:str)->N:={return(sys.getFunction("getUniform1i"))}
    Transit "" getUniformMatrix2x2f(N:this_,N:str,N:v)->N:={return(sys.getFunction("getUniformMatrix2x2f"))}
    Transit "" getUniformMatrix3x3f(N:this_,N:str,N:v)->N:={return(sys.getFunction("getUniformMatrix3x3f"))}
    Transit "" getUniformMatrix4x4f(N:this_,N:str,N:v)->N:={return(sys.getFunction("getUniformMatrix4x4f"))}
}


Framework{
    Framework_Main()->N := {
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