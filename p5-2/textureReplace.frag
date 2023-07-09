#version 450 core

uniform sampler2D texUnit0;

uniform int swapColor;
uniform float TVflag;
uniform float bright;
uniform float timeV;
uniform int noiseIO;
uniform bool alphaTime;
uniform int tilePPF;
uniform float zPPF, yPPF, xPPF;

in vec2 texCoordFrag;
in vec3 colorFrag;

out vec4 fragColor;

float rand(float n){return fract(sin(n) * 43758.5453123);}

float noiseOne(float p){
	float fl = floor(p);
  float fc = fract(p);
	return mix(rand(fl), rand(fl + 1.0), fc);
}
	
float random (vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

vec2 truchetPattern(in vec2 p, in float _index){
    _index = fract(((_index-0.5)*2.0));
    if (_index > 0.75) {
        p = vec2(1.0) - p;
    } else if (_index > 0.5) {
        p = vec2(1.0-p.x,p.y);
    } else if (_index > 0.25) {
        p = 1.0-vec2(1.0-p.x,p.y);
    }
    return p;
}

vec2 tileF (vec2 p, float z)
{
    p*=z;
    return fract(p);
}

vec2 radialTile(vec2 p, vec2 speed, float zoom){
  vec2 toExtreme = vec2(0.5)-p;
  vec2 polar = vec2( (3.14+atan(toExtreme.y,toExtreme.x))/(2*3.14),   // Angulo
                      log(length(toExtreme))*1.62*0.1);            // Radius
  polar *= zoom;

  polar.y += speed.y;
  if (fract( polar.y * 0.5) > 0.5){
    polar.x += speed.x;
  } else {
    polar.x -= speed.x;
  }

  return fract(polar);
}

float chaos (float timeX)
{
    return 3.756918*timeX*(1-timeX)+0.6;
}

float star (vec2 p, float w)
{
  float pct0 = smoothstep(p.x-w,p.x,p.y);
  pct0 *= 1.0-smoothstep(p.x,p.x+w,p.y);

  float pct1 = smoothstep(p.x-w,p.x,1.0-p.y);
  pct1 *= 1.-smoothstep(p.x,p.x+w,1.0-p.y);

  return pct0+pct1;
}

vec2 brickTile(vec2 p, float zoom){
    p *= zoom;
    if (fract(p.y * 0.5) > 0.5){
        p.x += 0.5;
    }
    return fract(p);
}

float box(vec2 p, vec2 size){
    size = vec2(0.5)-size*0.5;
    vec2 uv = smoothstep(size,size+vec2(0.0001),p);
    uv *= smoothstep(size,size+vec2(0.0001),vec2(1.0)-p);
    return uv.x*uv.y;
}

vec2 rotate2D(vec2 p, float angle){
  p -= 0.5;
  p =  mat2(cos(angle),-sin(angle),
              sin(angle),cos(angle)) * p;
  p += 0.5;
  return p;
}

float circle(vec2 p, float radius){
  vec2 pos = vec2(0.5)-p;
  return smoothstep(1.0-radius,1.0-radius+radius*0.2,1.-dot(pos,pos)*3.14);
}

float box2(vec2 p, vec2 size, float smoothEdges){
  size = vec2(0.5)-size*0.5;
  vec2 aa = vec2(smoothEdges*0.5);
  vec2 uv = smoothstep(size,size+aa,p);
  uv *= smoothstep(size,size+aa,vec2(1.0)-p);
  return uv.x*uv.y;
}

vec2 offset(vec2 p, vec2 offset){
  vec2 uv;
  if(p.x>0.5){
    uv.x = p.x - 0.5;
  } else {
    uv.x = p.x + 0.5;
  } 
  if(p.y>0.5){
    uv.y = p.y - 0.5;
  }else {
    uv.y = p.y + 0.5;
  } 
  return uv;
}

vec2 movingTiles(vec2 p, float zoom, float speed){
  p *= zoom;
  float time = timeV*speed;
  if( fract(time)>0.5 ){
    if (fract( p.y * 0.5) > 0.5){
      p.x += fract(time)*2.0;
    } else {
      p.x -= fract(time)*2.0;
    } 
  } else {
    if (fract( p.x * 0.5) > 0.5){
      p.y += fract(time)*2.0;
    } else {
        p.y -= fract(time)*2.0;
    } 
  }
  return fract(p);
}

void main() { 
    vec4 colorM = vec4(colorFrag,1.0f);
    vec4 unit0 = texture(texUnit0, texCoordFrag);
    vec3 bw = vec3(0.3f,0.59f,0.11f);
    vec3 gry = vec3(0.2f,0.2f,0.2f);
    vec3 red = vec3(1.0f,0.0f,0.0f);
    vec3 blc = vec3(0.0f,0.0f,0.0f);
    vec3 wht = vec3(1.0f,1.0f,1.0f);
    float alpha = dot(unit0.xyz,bw);

    alpha*=10;
    alpha=alpha>1.0f?1.0f:alpha;
    alpha=alpha<0.0f?0.0f:alpha;

    float sum = unit0.x+unit0.y+unit0.z;

    if (sum<=0.03||unit0.xyz==red||unit0.xyz==blc)
    {
    discard;
    }

      switch(int(swapColor))
      {
      case 0:
        break;
      case 1:
        unit0*=vec4(vec3(1,0,1),1);
        break;
      case 2:
        unit0*=vec4(vec3(1,1,0),1);
        break;
      case 3:
        unit0*=vec4(vec3(1,0,0),1);
        break;
      case 4:
        unit0*=vec4(vec3(0.2),1);
        break;
      case 5:
        unit0*=vec4(vec3(0.5,0.5,0.5),1);
        break;
      case 6:
        unit0*=vec4(bw,1);
        break;
      default:
        break;
      };

  bool flag = bool(TVflag);
  if (flag)
  {
  unit0=texture(texUnit0, texCoordFrag);
  float convertedColor = dot(unit0.xyz, bw);
  vec3 scanlines = vec3(0.2, 0.8, 1.0);
  int line = int(gl_FragCoord.y)%4;
  switch(line)
  {
  case 0:
    convertedColor*=scanlines.x;
    break;
  case 1:
    convertedColor*=scanlines.x;
    break;
  case 2:
    convertedColor*=scanlines.y;
    break;
  case 3:
    convertedColor*=scanlines.z;
    break;
  default:
    convertedColor*=1.0;
    break;
  };
  unit0=vec4(vec3(convertedColor),1);
  };

  vec2 col2 = texCoordFrag;
  vec2 ipos = vec2(0,0);
  vec2 fpos = vec2(0,0);
  vec2 tile = vec2(0,0);
  float color= 0.0f;
  switch (noiseIO)
  {
  case 0:
    break;
  case 1:
    col2 = tileF(col2, tilePPF);
    unit0.x = star(col2,xPPF);
    unit0.y = star(col2,yPPF);
    unit0.z = star(col2,zPPF);
    break;
  case 2:
    col2 = brickTile(col2,tilePPF);
    unit0.x = box(col2,vec2(xPPF));
    unit0.y = box(col2,vec2(yPPF));
    unit0.z = box(col2,vec2(zPPF));
    break;
  case 3:
    col2 = tileF(col2, tilePPF);
    unit0.x = box(col2,vec2(xPPF));
    unit0.y = box(col2,vec2(yPPF));
    unit0.z = box(col2,vec2(zPPF));
    break;
  case 4:
    col2 = tileF(col2, tilePPF);
    vec2 offsetCol2 = offset(col2,vec2(0.5));
    col2 = rotate2D(col2,3.14*0.25);
    unit0.x = box2(offsetCol2,vec2(xPPF),0.01) - box2(col2,vec2(yPPF),0.01) + 2.*box2(col2,vec2(zPPF),0.01);
    unit0.y = box2(offsetCol2,vec2(xPPF),0.01) - box2(col2,vec2(yPPF),0.01) + 2.*box2(col2,vec2(zPPF),0.01);
    unit0.z = box2(offsetCol2,vec2(xPPF),0.01) - box2(col2,vec2(yPPF),0.01) + 2.*box2(col2,vec2(zPPF),0.01);
    break;
  case 5:
    col2 = tileF(col2, tilePPF);
    col2 = rotate2D(col2,3.14*0.25);
    unit0.x = box2(col2,vec2(xPPF),0.01);
    unit0.y = box2(col2,vec2(yPPF),0.01);
    unit0.z = box2(col2,vec2(zPPF),0.01);
    break;
  case 6:
    col2 = brickTile(col2,tilePPF);
    float angle = 3.14*0.25*cos(timeV*0.5);
    float rows = 10;
    if (fract(texCoordFrag.y * 0.5 * rows) > 0.5)
    {
      angle *= -1.0;
    }
    col2 = rotate2D(col2,angle);
    col2 *= 2.0;
    float pct = (1.0+cos(3.14*col2.x))*0.5;
    unit0.x = 1.0-smoothstep( xPPF,yPPF, pow(pct,col2.y) ) * 1.0-smoothstep( yPPF,zPPF, pow(pct,2.0-col2.y) );
    unit0.y = 1.0-smoothstep( xPPF,yPPF, pow(pct,col2.y) ) * 1.0-smoothstep( yPPF,zPPF, pow(pct,2.0-col2.y) );
    unit0.z = 1.0-smoothstep( xPPF,yPPF, pow(pct,col2.y) ) * 1.0-smoothstep( yPPF,zPPF, pow(pct,2.0-col2.y) );
    break;
  case 7:
    col2 = rotate2D(col2,3.14*fract(timeV*0.1));
    //col2 = brickTile(col2,tilePPF);
    col2 = radialTile(col2,vec2(fract(timeV*0.5)),tilePPF+5);
    col2 = radialTile(col2,vec2(-fract(timeV),0.0),tilePPF+2);
    unit0.x = box2(col2,vec2(xPPF,1 - xPPF),0.01);
    unit0.y = box2(col2,vec2(yPPF,1 - yPPF),0.01);
    unit0.z = box2(col2,vec2(zPPF,1 - zPPF),0.01);
    break;
  case 8:
    unit0.x = min (1, xPPF*chaos(timeV));
    unit0.y = min (1, yPPF*chaos(timeV));
    unit0.z = min (1, zPPF*chaos(timeV));
    break;
  case 9:
    unit0.x = min (1, xPPF*random(col2));
    unit0.y = min (1, yPPF*random(col2));
    unit0.z = min (1, zPPF*random(col2));
    break;
  case 10:
    col2 *= tilePPF;
    ipos = floor(col2);
    unit0.x = min (1, xPPF*random( ipos ));
    unit0.y = min (1, yPPF*random( ipos ));
    unit0.z = min (1, zPPF*random( ipos ));
    break;
  case 11:
    col2 *= tilePPF;
    fpos = fract(col2);
    unit0.x = min (1, xPPF*fpos.x);
    unit0.y = min (1, yPPF*fpos.y);
    unit0.z = min (1, zPPF*0.01);
    break;
  case 12:
    col2 *= tilePPF;
    ipos = floor(col2);
    fpos = fract(col2);
    vec2 tile = truchetPattern(fpos, random( ipos ));
    color = smoothstep(tile.x-0.3,tile.x,tile.y)-
            smoothstep(tile.x,tile.x+0.3,tile.y);
    unit0.x = min (1, xPPF*color);
    unit0.y = min (1, yPPF*color);
    unit0.z = min (1, zPPF*color);
    break;
  case 13:
    col2 *= tilePPF;
    col2 = (col2-vec2(5.0))*(abs(sin(timeV*0.2))*5.);
    col2.x += timeV*3.0;
    ipos = floor(col2);
    fpos = fract(col2);
    tile = truchetPattern(fpos, random( ipos ));
    color = (step(length(tile),0.6) -
              step(length(tile),0.4) ) +
             (step(length(tile-vec2(1.)),0.6) -
              step(length(tile-vec2(1.)),0.4) );
    unit0.x = min (1, xPPF*color);
    unit0.y = min (1, yPPF*color);
    unit0.z = min (1, zPPF*color);
    break;
  case 14:
    col2 *= tilePPF;
    col2 = (col2-vec2(5.0))*(abs(sin(timeV*0.2))*5.);
    col2.x += timeV*3.0;
    ipos = floor(col2);
    fpos = fract(col2);
    tile = truchetPattern(fpos, random( ipos ));
    unit0.x = min (1, xPPF*step(tile.x,tile.y));
    unit0.y = min (1, yPPF*step(tile.x,tile.y));
    unit0.z = min (1, zPPF*step(tile.x,tile.y));
    break;
  default:
    break;
  }

  unit0.x=min(1, unit0.x*bright);
  unit0.y=min(1, unit0.y*bright);
  unit0.z=min(1, unit0.z*bright);
  if (alphaTime) alpha = 1;
  fragColor = vec4(unit0.xyz,alpha);
}
