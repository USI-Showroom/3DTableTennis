#ifndef COLORDEF_H
#define COLORDEF_H

/***************************************************************************
ColorDef.h
Comment:  This file contains all color definitions.
***************************************************************************/

//-----------------------------------------------------------------------------
/// RGB color definition.
/**
* This class handles a RGB color with a value for the red, the green and
* the blue part of the color. All values are unsigned char (0-255).
*/
class Color3 {

//-----------------------------------------------------------------------------
public:

  enum PredefColor 
  {
    WHITE,
    BLACK,
    RED,
    GREEN,
    BLUE,
    YELLOW,
    CYAN,
    MAGENTA,
    GREY,
    DARKRED,
    DARKGREEN,
    DARKBLUE,
    DARKYELLOW,
    DARKCYAN,
    DARKMAGENTA,
    DARKGREY
  };

  /**
  * Standard constructor for a color. Color will be set to grey. 
  */
  Color3() : r( 70 ), g( 70 ), b( 70 ) {};

  /**
  * Constructor with predifined color.
  * @param c - the color number
  */
  Color3( PredefColor c ) 
  {
    switch( c )
    {
      case  0 : setRGB( 255, 255, 255 ); break;
      case  1 : setRGB(   0,   0,   0 ); break;
      case  2 : setRGB( 255,   0,   0 ); break;
      case  3 : setRGB(   0, 255,   0 ); break;
      case  4 : setRGB(   0,   0, 255 ); break;
      case  5 : setRGB( 255, 255,   0 ); break;
      case  6 : setRGB(   0, 255, 255 ); break;
      case  7 : setRGB( 255,   0, 255 ); break;
      case  8 : setRGB( 160, 160, 160 ); break;
      case  9 : setRGB( 128,   0,   0 ); break;
      case 10 : setRGB(   0, 128,   0 ); break;
      case 11 : setRGB(   0,   0, 128 ); break;
      case 12 : setRGB( 128, 128,   0 ); break;
      case 13 : setRGB(   0, 128, 128 ); break;
      case 14 : setRGB( 128,   0, 128 ); break;
      case 15 : setRGB( 128, 128, 128 ); break;
      default: setRGB(   0,   0,   0 );
    }
  };

  /**
  * Constructor for a color. Color will be set to the given RGB values.
  * @param red - red value
  * @param green - green value
  * @param blue - blue value
  */
  Color3( int red, int green, int blue )
    : r( red ), g( green ), b( blue ) {};


  /**
  * Copy ctor for a color. Color will be set to the given RGB values.
  * @param red - red value
  * @param green - green value
  * @param blue - blue value
  */
  Color3( const Color3& col  )
    :r( col.red() ), g( col.green() ), b( col.blue() ) {};

  /* Set the RGB values of this color.
  * @param r - the red part of the color
  * @param g - the green part of the color
  * @param b - the blue part of the color
  */
  void setRGB( const int red, const int green, const int blue ) {
    r = red; g = green; b = blue;
  };

  /**
  * Get the red part of the color.
  * @return the <b> red part </b> of the color
  */
  unsigned char& red() { return r; };

  /**
  * Get the red part of the color (constant).
  * @return the <b> red part </b> of the color
  */
  const unsigned char& red() const { return r; };

  /**
  * Get the green part of the color.
  * @return the <b> green part </b> of the color
  */
  unsigned char& green() { return g; };

  /**
  * Get the green part of the color (constant).
  * @return the <b> green part </b> of the color
  */
  const unsigned char& green() const { return g; };

  /**
  * Get the blue part of the color.
  * @return the <b> blue part </b> of the color
  */
  unsigned char& blue() { return b; };

  /**
  * Get the blue part of the color (constant).
  * @return the <b> blue part </b> of the color
  */
  const unsigned char& blue() const { return b; };

  /**
  * Operator that returns the red for i == 0, green for i == 1
  * or blue part of the color for i == 2.
  * @param i - index of the color part
  * @return the <b> color part </b> at index \em i
  */
  unsigned char& operator [] ( const int i ) {
    assert( i < 3 );
    if ( i == 0 )
      return r;
    if ( i == 1 )
      return g;
      
    return b;
  };


  /**
  * Operator that assigns the color with a given double value and 
  * returns the resuling color.
  * @param col - the other color
  * @return the <b> new color </b>
  */
  void operator = ( const Color3& col ) {
    r = col.red();
    g = col.green();
    b = col.blue();
  };

  /**
  * Operator that multiplies the color with a given double value and 
  * returns the resuling color.
  * @param w - the multiplier
  * @return the <b> new color </b>
  */
  const Color3 operator * ( const double w ) const {
    return Color3( (int)( r * w ), (int)( g * w ), (int)( b * w ) );
  };

  /**
  * Operator that adds a color to this color and 
  * returns the resuling color.
  * @param col - the addend
  * @return \b sum of the colors
  */
  const Color3 operator + ( const Color3& col ) const {
    return Color3( r + col.red(), g + col.green(), b + col.blue() );
  };

  /**
  * Interpolate colors (set via mode) using the given values for min, max and value
  * and set the result to this color.
  * @param minT - the minimum
  * @param maxT - the maximum
  * @param value - the value
  * @param mode - the interpolation colors
  */
  void colorRamp( const double minT, const double maxT, const double value,
    const int mode = 0 );

  /**
  * Interpolate the given colors using the given values for min, max and value
  * and set the result to this color.
  * @param minT - the minimum
  * @param maxT - the maximum
  * @param value - the value
  * @param colors - the interpolation colors
  */
  void colorRamp( const double minT, const double maxT, const double value,
    const std::vector < Color3 >& colors );

  /**
  * Average the color and the given color.
  * @param c - the color
  * @param numAvg - the number of done averages
  */
  void average( const Color3& c, int& numAvg ) {
    const int old = numAvg++;
    if ( !old ) *this = c;
    const double w = 1.0 / numAvg;
    r = (int)( ( r * old + c.red() ) * w );
    g = (int)( ( g * old + c.green() ) * w );
    b = (int)( ( b * old + c.blue() ) * w );
  };

  /**
  * Set a random color.
  * Warning: no srand() init is done!
  */
  void setRandom() {
    r = rand() % 256; g = rand() % 256; b = rand() % 256;
  };

  /**
  * Get a random color.
  */
  static const Color3 Random() {
    return Color3( rand() % 256, rand() % 256, rand() % 256 );
  };


//-----------------------------------------------------------------------------
private:

  /** The red part of the color. */
  unsigned char r;
  /** The green part of the color. */
  unsigned char g;
  /** The blue part of the color. */
  unsigned char b;

};


//-----------------------------------------------------------------------------
/// RGBA color definition.
/**
* This class handles a RGBA color with a value for the red, the green and
* the blue part of the color. All values are float (0.0 - 1.0).
*/
class Color4f {

//-----------------------------------------------------------------------------
public:

  /**
  * Standard construtor for a RGBA color. Color will be set to opaque black. 
  */
  Color4f() : r( 0.0f ), g( 0.0f ), b( 0.0f ), a( 1.0f ) {};

  /**
  * Constructor for a color. Color will be set to the given values for 
  * red, green and blue. The alpha value is set to opaque.
  * @param red - red value
  * @param green - green value
  * @param blue - blue value
  */
  Color4f( const float red, const float green, const float blue )
    : r( red ), g( green ), b( blue ), a( 1.0f ) {};

  /**
  * Constructor for a color. Color will be set to the given RGBA values.
  * @param red - red value
  * @param green - green value
  * @param blue - blue value
  * @param alpha - alpha value
  */
  Color4f( const float red, const float green, const float blue, const float alpha )
    : r( red ), g( green ), b( blue ), a( alpha ) {};

  /**
  * Get the red part of the color.
  * @return the <b> red part </b> of the color
  */
  float& red() { return r; };

  /**
  * Get the red part of the color (constant).
  * @return the <b> red part </b> of the color
  */
  const float& red() const { return r; };

  /**
  * Get the green part of the color.
  * @return the <b> green part </b> of the color
  */
  float& green() { return g; };

  /**
  * Get the green part of the color (constant).
  * @return the <b> green part </b> of the color
  */
  const float& green() const { return g; };

  /**
  * Get the blue part of the color.
  * @return the <b> blue part </b> of the color
  */
  float& blue() { return b; };

  /**
  * Get the blue part of the color (constant).
  * @return the <b> blue part </b> of the color
  */
  const float& blue() const { return b; };

  /**
  * Get the alpha value of the color.
  * @return the <b> alpha value </b> of the color
  */
  float& alpha() { return a; };

  /**
  * Get the alpha value of the color (constant).
  * @return the <b> alpha value </b> of the color
  */
  const float& alpha() const { return a; };

  /**
  * Operator that returns the red for i == 0, green for i == 1,
  * blue for i == 2 or alpha part of the color for i == 3.
  * @param i - index of the color part
  * @return the <b> color part </b> at index \em i
  */
  float& operator [] ( const int i ) {
    assert( i < 4 );
    if ( i == 0 )
      return r;
    if ( i == 1 )
      return g;
    if ( i == 2 )
      return b;

    return a;
  };

  /**
  * Operator that returns the red for i == 0, green for i == 1,
  * blue for i == 2 or alpha part of the color for i == 3 (constant).
  * @param i - index of the color part
  * @return the <b> color part </b> at index \em i
  */
  const float& operator [] ( const int i ) const {
    assert( i < 4 );
    if ( i == 0 )
      return r;
    if ( i == 1 )
      return g;
    if ( i == 2 )
      return b;
    
    return a;
  };


//-----------------------------------------------------------------------------
private:

  /** The red part of the color. */
  float r;
  /** The green part of the color. */
  float g;
  /** The blue part of the color. */
  float b;
  /** The alpha value of the color. */
  float a;

};

#endif