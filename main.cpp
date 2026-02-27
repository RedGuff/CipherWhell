#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

constexpr double PI = 3.14159265358979323846;

bool initialHoleHere = true;
int numberRing = 0;

struct Ring {
    std::string text;
    double ringWidth;     // largeur radiale en mm
    double spacingAfter;  // espace après cet anneau en mm
};

struct Config {
    double outerDiameter; // en mm
    double fontSize;      // en mm
    std::string output;
    std::vector<Ring> rings;
};

std::pair<double,double> polar(double cx, double cy,
                               double r, double angle)
{
    return {
        cx + r * std::cos(angle),
        cy + r * std::sin(angle)
    };
}

void drawRing(std::ofstream& file,
              double cx, double cy,
              double Rext, double Rint,
              const std::string& text,
              double fontSize)
{
    size_t n = text.size();
    if (n == 0) return;

    double step = 2.0 * PI / n;

    for (size_t i = 0; i < n; ++i)
    {
        double a1 = i * step - PI/2.0;
        double a2 = (i+1) * step - PI/2.0;
        double amid = (a1 + a2) / 2.0;

        auto [x1,y1] = polar(cx,cy,Rext,a1);
        auto [x2,y2] = polar(cx,cy,Rext,a2);
        auto [x3,y3] = polar(cx,cy,Rint,a2);
        auto [x4,y4] = polar(cx,cy,Rint,a1);

        file << "<path d=\""
             << "M " << x1 << " " << y1 << " "
             << "A " << Rext << " " << Rext << " 0 0 1 "
             << x2 << " " << y2 << " "
             << "L " << x3 << " " << y3 << " "
             << "A " << Rint << " " << Rint << " 0 0 0 "
             << x4 << " " << y4 << " Z\" "
             << "fill=\"none\" stroke=\"black\" stroke-width=\"0.3\"/>\n";

              unsigned char c = static_cast<unsigned char>(text[i]); // Ok for & and other special character.

if (initialHoleHere && (numberRing == 2) && (static_cast<unsigned int>(c) == 32 )){
        int bonusRadius = 2;
 auto [x3,y3] = polar(cx,cy,Rint+bonusRadius,a2);
        auto [x4,y4] = polar(cx,cy,Rint+bonusRadius,a1);

        file << "<path d=\""
             << "M "
            << x3 << " " << y3 << " "
             << "A " << Rint + bonusRadius << " " << Rint + bonusRadius << " 0 0 0 "
             << x4 << " " << y4 << "\" "
             << "fill=\"none\" stroke=\"black\" stroke-width=\"3\"/>\n";
}
   float fontHighCoef = 1.0;
if (static_cast<unsigned int>(c) == 44) // "," for comma, apostrophe, half ", diacritical on the next character...
{
fontHighCoef = 7.0;
}
else
{
    fontHighCoef = 1.0;
}
        double rText = (fontHighCoef*Rext + Rint)/(fontHighCoef + 1.0);
        auto [tx,ty] = polar(cx,cy,rText,amid);
        double angleDeg = amid * 180.0 / PI + 90.0;
       file << "<text x=\"" << tx << "\" y=\"" << ty << "\" ";
           float fontSizeCoef = 1.0;
if (static_cast<unsigned int>(c) == 44) // "," for comma, apostrophe, half ", diacritical on the next character...
{
fontSizeCoef = 2.0;
}
else
{
    fontSizeCoef = 1.0;
}
           file  << "font-size=\"" << fontSize*fontSizeCoef << "mm\" "
             << "text-anchor=\"middle\" "
             << "dominant-baseline=\"middle\" "
             << "transform=\"rotate(" << angleDeg
             << " " << tx << " " << ty << ")\">"
             << "&#" << static_cast<unsigned int>(c) << ";"
             << "</text>\n";
    }
}

void generateSVG(const Config& cfg)
{
    constexpr double pageWidth  = 210.0;
    constexpr double pageHeight = 297.0;

    double cx = pageWidth  / 2.0;
    double cy = pageHeight / 2.0;

    double Rcurrent = cfg.outerDiameter / 2.0;

    std::ofstream file(cfg.output);
    file << std::fixed << std::setprecision(3);

    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<svg xmlns=\"http://www.w3.org/2000/svg\" "
         << "width=\"210mm\" height=\"297mm\" "
         << "viewBox=\"0 0 210 297\">\n";

    file << "<rect width=\"100%\" height=\"100%\" fill=\"white\"/>\n";

    for (const auto& ring : cfg.rings)
    {
        numberRing = numberRing +1;
        double Rext = Rcurrent;
        double Rint = Rext - ring.ringWidth;

        drawRing(file, cx, cy, Rext, Rint,
                 ring.text, cfg.fontSize);

        Rcurrent = Rint - ring.spacingAfter;
    }

 // Space between rotor and stator:
 int radiuscut = 60; // (cfg.outerDiameter / 2) - ringWidth1 - ringWidth2;
 file << "<circle cx=\"" << cx
         << "\" cy=\"" << cy
         << "\"  r=\""<< radiuscut << "\"  style=\"stroke:black; fill:none\" />\n";

    // Center hole:
    file << "<circle cx=\"" << cx
         << "\" cy=\"" << cy
         << "\" r=\"0.2\" fill=\"black\" />\n";

    file << "</svg>\n";
}

int main()
{
    Config cfg;
    cfg.outerDiameter = 180.0; // Good in a A4 paper.
    cfg.fontSize = 3.0;        // In mm.
    cfg.output = "triple_A4.svg";

    cfg.rings = {
        {"1234567890                      ", 15.0},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ .,&#;", 15.0},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ .,&#;", 15.0}
    };

    generateSVG(cfg);
    std::cout << "SVG A4 Ok.\n";
}
