/* habcat.cpp */

/*
This command line utility reads the HabCat Table 4 data that has
been preprocessed into comma separated values (SCV) via import/export
conversion through Microsoft Excel. This program reads the input CSV
and converts the data into an alterative form with some calculated
or estimated values. The converted data is written to a CSV file.
*/

#include <cstdio>
#include <cmath>
#include <string>
#include <vector>

using namespace std;

struct HabStar {
    // From file
    size_t hip; // Hipparcos number
    double rah; // Right ascension hours
    double ram; // Right ascension minutes
    double ras; // Right ascension seconds
    double ded; // Declination degrees
    double dem; // Declination arcminutes
    double des; // Declination arcseconds
    double vmag; // V-band apparent magnitude
    double plx; // Parallax (milliarcseconds)
    double plxe; // Parallax standard error (milliarcseconds)
    double bv; // B-V color index
    double bve; // B-V color index standard error
    string ccdm; // Catalogue of Components of Double and Multiple Star name
    size_t hd; // Henry Draper catalog number
    string bd; // Bonner Durchmusterung catalog name

    // Computed
    double ra; // Right ascension in decimal degrees
    double dec; // Declination in decimal degrees
    double dist; // Distance in light years
};

class Util {
public:

static void ProcessCommandLine(int argc, char** argv, string& inFileName, string& outFileName) {
    // TBD
    if (argc == 1) return;
    --argc, ++argv;

    const string inCmd = "-i";
    const string outCmd = "-o";

    while (argc) {
        if (inCmd == *argv && argc > 1) {
            --argc, ++argv;
            inFileName = string(*argv);
            --argc, ++argv;
            continue;
        } else if (outCmd == *argv && argc > 1) {
            --argc, ++argv;
            outFileName = string(*argv);
            --argc, ++argv;
            continue;
        } else {
            // on unexpected argument
            fprintf(stderr, "Usage: habcat [-i inFileName] [-o outFileName]\n");
            return;
        }
    }
}

static void Split(const string& in, const string& delim, vector<string>& out) {
    string tmp;

    out.clear();
    if (delim.length() == 0) return;

    string::const_iterator i = in.cbegin();
    while (i != in.cend())  {
        bool found = false;
        for (string::const_iterator d = delim.cbegin(); d != delim.cend(); ++d) {
            if (*i == *d) {
                out.push_back(tmp);
                tmp.clear();
                ++i;
                found = true;
                break;
            }
        }
        if (!found) {
            tmp.append(i, i+1);
            ++i;
        }
    }
    if (tmp.length() > 0) out.push_back(tmp);
}

};

int main (int argc, char** argv) {
    string inFileName = "datafile4.csv";
    FILE* in;
    string outFileName = "myhabcat.csv";
    FILE* out;

    // Parse command line arguments
    Util::ProcessCommandLine(argc, argv, inFileName, outFileName);

    // Open files
    in = fopen(inFileName.c_str(), "r");
    if (in == NULL) {
        fprintf(stderr, "Can't open %s for reading: %s\n", inFileName.c_str(), strerror(errno));
        return errno;
    }
    out = fopen(outFileName.c_str(), "w");
    if (out == NULL) {
        fprintf(stderr, "Can't open %s for writing: %s\n", inFileName.c_str(), strerror(errno));
        fclose(in);
        return errno;
    }

    // Skip header of input file
    string line;
    const size_t MAXLINE = 1024;
    char buf[MAXLINE];

    fgets(buf, MAXLINE, in);

    // Write the header to the output file
    fprintf(out, "Hip ID,RA,Dec,Dist\n");

    // Read each line of the input file and process
    while (!feof(in)) {
        line.clear();
        fgets(buf, MAXLINE, in);
        line.append(buf);

        // Fill in record
        HabStar star;
        vector<string> fields;
        Util::Split(line, ",\n", fields);
        if (fields.size() != 15) {
            fprintf(stderr, "Malformed line in %s\n", inFileName.c_str());
            break;
        }
        vector<string>::const_iterator i = fields.cbegin();
        star.hip = static_cast<size_t>(atoi(i->c_str())); i++;
        star.rah = atof(i->c_str()); i++;
        star.ram = atof(i->c_str()); i++;
        star.ras = atof(i->c_str()); i++;
        star.ded = atof(i->c_str()); i++;
        star.des = atof(i->c_str()); i++;
        star.dem = atof(i->c_str()); i++;
        star.vmag = atof(i->c_str()); i++;
        star.plx = atof(i->c_str()); i++;
        star.plxe = atof(i->c_str()); i++;
        star.bv = atof(i->c_str()); i++;
        star.bve = atof(i->c_str()); i++;
        star.ccdm = *i; i++;
        star.hd = static_cast<size_t>(atoi(i->c_str())); i++;
        star.bd = *i;
        // To be calculated
        star.ra = 0;
        star.dec = 0;
        star.dist = 0;

        // Calculations
        star.ra = 180 - ((star.rah * 15.0) + (star.ram / 2.0) + (star.ras / 240.0));
        star.dec = (fabs(star.ded) + (star.dem / 60.0) + (star.des / 3600.0)) * ((star.ded < 0) ? -1 : 1);
        star.dist = 3.261563777 / (star.plx / 1000.0); // in light years
        // more tbd

        // tbd
        fprintf(out, "Hip%d,%8.5f,%8.5f,%8.5f\n", star.hip, star.ra, star.dec, star.dist);
    }

    // Close files
    fclose(in);
    fclose(out);

    return 0;
}
