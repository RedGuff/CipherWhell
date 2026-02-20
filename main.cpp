#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

constexpr double PI = 3.14159265358979323846;

struct Config {
    std::string text;
    double outerDiameter;   // diamètre extérieur
    double ringWidth;       // largeur de la couronne
    double fontSize;
    std::string output;
};

std::pair<double,double> polarToCartesian(double cx, double cy,
                                          double r, double angle)
{
    return {
        cx + r * std::cos(angle),
        cy + r * std::sin(angle)
    };
}

void generateSVG(const Config& cfg)
{
    const size_t n = cfg.text.size();
    if (n == 0) return;

    const double Rext = cfg.outerDiameter / 2.0;
    const double Rint = Rext - cfg.ringWidth;
    const double cx = Rext + 10;
    const double cy = Rext + 10;
    const double svgSize = cfg.outerDiameter + 20;

    const double angleStep = 2.0 * PI / n;

    std::ofstream file(cfg.output);
    if (!file) {
        std::cerr << "Erreur ouverture fichier\n";
        return;
    }

    file << std::fixed << std::setprecision(3);
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<svg xmlns=\"http://www.w3.org/2000/svg\" "
         << "width=\"" << svgSize << "\" "
         << "height=\"" << svgSize << "\">\n";

    file << "<rect width=\"100%\" height=\"100%\" fill=\"white\"/>\n";

    for (size_t i = 0; i < n; ++i) {

        double a1 = i * angleStep - PI/2.0;
        double a2 = (i+1) * angleStep - PI/2.0;
        double amid = (a1 + a2) / 2.0;

        // Points arc externe
        auto [x1, y1] = polarToCartesian(cx, cy, Rext, a1);
        auto [x2, y2] = polarToCartesian(cx, cy, Rext, a2);

        // Points arc interne
        auto [x3, y3] = polarToCartesian(cx, cy, Rint, a2);
        auto [x4, y4] = polarToCartesian(cx, cy, Rint, a1);

        int largeArc = 0; // toujours < 180° ici

        file << "<path d=\""
             << "M " << x1 << " " << y1 << " "
             << "A " << Rext << " " << Rext << " 0 "
             << largeArc << " 1 " << x2 << " " << y2 << " "
             << "L " << x3 << " " << y3 << " "
             << "A " << Rint << " " << Rint << " 0 "
             << largeArc << " 0 " << x4 << " " << y4 << " "
             << "Z\" fill=\"none\" stroke=\"black\"/>\n";

        // Position texte
        double rText = (Rext + Rint) / 2.0;
        auto [tx, ty] = polarToCartesian(cx, cy, rText, amid);

        double angleDeg = amid * 180.0 / PI + 90.0;
unsigned char c = static_cast<unsigned char>(cfg.text[i]);
        file << "<text x=\"" << tx << "\" y=\"" << ty << "\" "
             << "font-size=\"" << cfg.fontSize << "\" "
             << "text-anchor=\"middle\" "
             << "dominant-baseline=\"middle\" "
             << "transform=\"rotate(" << angleDeg
             << " " << tx << " " << ty << ")\">"
             << "&#" << static_cast<unsigned int>(c) << ";" // Ok for & and other special character.
             << "</text>\n";
    }

    file << "</svg>\n";
}

int main()
{
    Config cfg{
        .text = "ABCDEFGHIJKLMNOPQRSTUVWXYZ .,&#;",
        .outerDiameter = 500.0,
        .ringWidth = 80.0,
        .fontSize = 28.0,
        .output = "cipher_dial.svg"
    };

    generateSVG(cfg);

    std::cout << "Fichier inscrit : " << cfg.output << "\n";
    return 0;
}
