/*
 * Copyright (C) 2019 Computer Graphics Group, University of Siegen
 * Written by Martin Lambers <martin.lambers@uni-siegen.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

uniform sampler2D tex0, tex1, tex2;

// these values are shared with frame.hpp!
const int ColorSpaceNone        = 0;
const int ColorSpaceLinearGray  = 1;
const int ColorSpaceLinearRGB   = 2;
const int ColorSpaceSLum        = 3;
const int ColorSpaceSRGB        = 4;
const int ColorSpaceY           = 5;
const int ColorSpaceXYZ         = 6;
uniform int colorSpace;
uniform int textureColorSpace;
uniform bool haveAlpha;
uniform sampler2D alphaTex;

uniform float minVal;
uniform float maxVal;
uniform float visMinVal;
uniform float visMaxVal;

uniform bool colorMap;
uniform sampler2D colorMapTex;

uniform bool magGrid;

smooth in vec2 vtexcoord;

layout(location = 0) out vec4 fcolor;

float normalizeVal(float x)
{
    return (x - minVal) / (maxVal - minVal);
}

const vec3 d65_xyz = vec3(95.047, 100.000, 108.883);

vec3 adjust_y(vec3 xyz, float new_y)
{
    float sum = xyz.x + xyz.y + xyz.z;
    // keep old chromaticity in terms of x, y
    float x = xyz.x / sum;
    float y = xyz.y / sum;
    // apply new luminance
    float r = new_y / y;
    return vec3(r * x, new_y, r * (1.0 - x - y));
}

vec3 rgb_to_xyz(vec3 rgb)
{
    return 100.0 * vec3(
            (0.4124 * rgb.r + 0.3576 * rgb.g + 0.1805 * rgb.b),
            (0.2126 * rgb.r + 0.7152 * rgb.g + 0.0722 * rgb.b),
            (0.0193 * rgb.r + 0.1192 * rgb.g + 0.9505 * rgb.b));
}

vec3 xyz_to_rgb(vec3 xyz)
{
    return 0.01f * vec3(
            (+3.2406255 * xyz.x - 1.5372080 * xyz.y - 0.4986286 * xyz.z),
            (-0.9689307 * xyz.x + 1.8757561 * xyz.y + 0.0415175 * xyz.z),
            (+0.0557101 * xyz.x - 0.2040211 * xyz.y + 1.0569959 * xyz.z));
}

float linear_to_snorm(float x)
{
    return (x <= 0.0031308 ? (x * 12.92) : (1.055 * pow(x, 1.0 / 2.4) - 0.055));
}

vec3 rgb_to_srgb(vec3 rgb)
{
    return vec3(linear_to_snorm(rgb.r), linear_to_snorm(rgb.g), linear_to_snorm(rgb.b));
}

void main(void)
{
    vec3 srgb;

    if (textureColorSpace == ColorSpaceNone) {
        // Get value
        float v = texture(tex0, vtexcoord).r;
        // Apply range selection
        v = (v - visMinVal) / (visMaxVal - visMinVal);
        v = clamp(v, 0.0, 1.0);
        // Apply color map
        if (colorMap) {
            srgb = texture(colorMapTex, vec2(v, 0.5)).rgb;
        } else {
            vec3 xyz = adjust_y(d65_xyz, 100.0 * v);
            srgb = rgb_to_srgb(xyz_to_rgb(xyz));
        }
    } else {
        // Get color
        vec3 xyz;
        if (textureColorSpace == ColorSpaceLinearGray) {
            float y = 100.0 * normalizeVal(texture(tex0, vtexcoord).r);
            xyz = adjust_y(d65_xyz, y);
        } else if (textureColorSpace == ColorSpaceLinearRGB) {
            float r = texture(tex0, vtexcoord).r;
            float g = texture(tex1, vtexcoord).r;
            float b = texture(tex2, vtexcoord).r;
            if (colorSpace == ColorSpaceSRGB) {
                r = normalizeVal(r);
                g = normalizeVal(g);
                b = normalizeVal(b);
            }
            xyz = rgb_to_xyz(vec3(r, g, b));
        } else if (textureColorSpace == ColorSpaceY) {
            float y = texture(tex0, vtexcoord).r;
            xyz = adjust_y(d65_xyz, y);
        } else if (textureColorSpace == ColorSpaceXYZ) {
            float x = texture(tex0, vtexcoord).r;
            float y = texture(tex1, vtexcoord).r;
            float z = texture(tex2, vtexcoord).r;
            xyz = vec3(x, y, z);
        }
        // Apply range selection
        float y = xyz.y;
        y = (y - visMinVal) / (visMaxVal - visMinVal);
        y = clamp(y, 0.0, 1.0);
        // Apply color map
        if (colorMap) {
            srgb = texture(colorMapTex, vec2(y, 0.5)).rgb;
        } else {
            xyz = adjust_y(xyz, 100.0 * y);
            vec3 rgb = xyz_to_rgb(xyz);
            if (haveAlpha) {
                float alpha = normalizeVal(texture(alphaTex, vtexcoord).r);
                alpha = clamp(alpha, 0.0, 1.0);
                rgb = clamp(rgb, vec3(0.0), vec3(1.0));
                rgb = rgb * alpha + vec3(1.0 - alpha);
            }
            srgb = rgb_to_srgb(rgb);
        }
    }

    // Apply grid
    if (magGrid) {
        vec2 texelCoord = vtexcoord * vec2(textureSize(tex0, 0));
        vec2 fragmentSizeInTexels = vec2(dFdx(texelCoord.x), dFdy(texelCoord.y));
        if (all(lessThan(fragmentSizeInTexels, vec2(0.5)))
                && any(lessThanEqual(fract(texelCoord), fragmentSizeInTexels))) {
            float l = dot(srgb, srgb);
            srgb = vec3(1.0 - l);
        }
    }
    fcolor = vec4(srgb, 1.0);
}
