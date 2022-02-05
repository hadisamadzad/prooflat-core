//=======================================
//==        In GOD We Trust
//==
//==          "proofLat"
//==    (Lateral Profile Drawer)
//==          version 1.6.1
//==              by
//==  Hadi Samadzad-Mohammad Sazegar
//==         December 2011
//=======================================

// Includes
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <string>

using namespace std;

// Structure Definement
struct point
{
    double x;
    double y;
};

struct line
{
    point A;
    point B;

    double Slope;
        // This parameter only uses in Make_Embank Function;
};

struct canal
{
    line Lines[5];
    bool Need;
};

struct paper
{
    line Lines[8];   //4 Lines for first border, 4 Lines for second border, 1 Line for Center Line (CL)
    double Scale;
    double width;
    double height;
    char Portrait_Landscape;
};

// Major Variables
double GL_Height, PL_Height;   // GL & PL Points
double GroundL_X,GroundR_X;   // Ground Left and Right X
double GroundL_Height, GroundR_Height;   // Ground Left and Right Y
double L_Slope, R_Slope;    // Away down is +
double L_Wide, R_Wide;  // The Length of Road's wide
double Lp_Slope, Rp_Slope;   // p is Shaane ye Raah
double Lp_Wide, Rp_Wide;
double z;   // Embank's Slope 1:z (H:V)
double Asphalt_Thick;

// Minor Variables
point GL, PL;
point Edge_L, Edge_R;   // Edge of Asphalt
point Edge_LL, Edge_RR;   // Down of Asphalt Thick
point Edge_LLL, Edge_RRR;   // Edge of Shaane ye Raah
#define CL_Line_Number 15
line Center_Line[CL_Line_Number];

point Intersects[2];    // Intersects of Handy Line & Embanks
                        // Uses In Make Hand Line & Make Embank & Find Intersects
//double L_Embank_Slope, R_Embank_Slope;
double Left_Handy_Slope, Right_Handy_Slope;
line L_Embank, R_Embank;

// Canal's Variables
canal Canals[4];
    /*
        Canal 0: Left Road Canal
        Canal 1: Right Road Canal
        Canal 2: Left Big Embank Canal
        Canal 3: Right Big Embank Canal
    */

// Shape Variables
line Lines[8];
    /*
        Line 0: Left Wide
        Line 1: Right Wide
        Line 2: Left Asphalt
        Line 3: Right Asphalt
        Line 4: Left Wide p
        Line 5: Right Wide p
        Line 6: Left Embank
        Line 7: Right Embank

        ** These lines used to drawing in DXF_Maker Function. Because they have array index
    */
string Station_Name;
paper A4_Paper;
int Ground_Points_Draw;
int Do_Calculate_Area;

#define Hand_Line_Point_Count 100
//#define Hand_Effect .01
line Hand_Line[Hand_Line_Point_Count / 2]; // For Left
line Hand_Line2[Hand_Line_Point_Count / 2]; // For Right

// Calculating Area Variables
double Cut_Area=0;    //Cut Area
double Fill_Area=0;    //Fill Area
point Points[4];    //Edges of Area

// Function's Prototypes
void InputAll();    // Inputs from File
void set_Inputs_to_Points();
void Prepare_Major_Lines();   // Prepares Road, Asphalt Thick & Shaane Lines
void Make_Hand_Line();   // Make's Handy drawn Line As Ground Line
void Canals_Need_Invest();   // Investigates that canal is needed or not
void Make_Small_Canals();   // Set's canal in 5 lines, Lines are as a line array
void Make_Big_Canals();
void Find_Intersects_and_Make_Embanks();  //Intersect of Handy Line and Embanks
void Calculate_Area();
void Make_Paper();
void Make_Report();
void MakeDXF();   // My Love !!!

//Main
//   #   #   #   #   #   #   #   #   #   #   #   #   #   #   #   #   #   #

int main()
{
    InputAll();
    set_Inputs_to_Points();
    Prepare_Major_Lines();
    Canals_Need_Invest();    //If Needed, calls Canal_Make
    Find_Intersects_and_Make_Embanks();

    if (Canals[2].Need || Canals[3].Need)
        Make_Big_Canals();

    Make_Paper();
    Make_Hand_Line();

    if (Do_Calculate_Area)
        Calculate_Area();

    Make_Report();
    MakeDXF();

    return 0;
}

//FUNCTIONS
//   #   #   #   #   #   #   #   #   #   #   #   #   #   #   #   #   #   #

void InputAll()
{
    fstream InputFile;

    InputFile.open("in",ios::in);

    InputFile >> Station_Name;

    InputFile >> GL_Height;
    InputFile >> PL_Height;

    InputFile >> GroundL_X;
    InputFile >> GroundL_Height;
    InputFile >> GroundR_X;
    InputFile >> GroundR_Height;

    InputFile >> L_Wide;
    InputFile >> L_Slope;
    InputFile >> R_Wide;
    InputFile >> R_Slope;

    InputFile >> Lp_Wide;
    InputFile >> Lp_Slope;
    InputFile >> Rp_Wide;
    InputFile >> Rp_Slope;

    InputFile >> Asphalt_Thick;
    InputFile >> z;

    InputFile >> Ground_Points_Draw;
    InputFile >> Do_Calculate_Area;

    InputFile.close();
}

void set_Inputs_to_Points()
{
    GL.x = 0;
    GL.y = GL_Height;

    PL.x = 0;
    PL.y = PL_Height;

    Edge_L.x = PL.x - L_Wide * cos(atan(L_Slope));
    Edge_L.y = PL.y - L_Wide * sin(atan(L_Slope));

    Edge_R.x = PL.x + R_Wide * cos(atan(R_Slope));
    Edge_R.y = PL.y - R_Wide * sin(atan(R_Slope));

    Edge_LL.x = Edge_L.x;
    Edge_LL.y = Edge_L.y - Asphalt_Thick;

    Edge_RR.x = Edge_R.x;
    Edge_RR.y = Edge_R.y - Asphalt_Thick;

    Edge_LLL.x = Edge_LL.x - Lp_Wide * cos(atan(Lp_Slope));
    Edge_LLL.y = Edge_LL.y - Lp_Wide * sin(atan(Lp_Slope));

    Edge_RRR.x = Edge_RR.x + Lp_Wide * cos(atan(Rp_Slope));
    Edge_RRR.y = Edge_RR.y - Lp_Wide * sin(atan(Rp_Slope));

}

void Prepare_Major_Lines()
{
    Lines[0].A = PL;
    Lines[0].B = Edge_L;

    Lines[1].A = PL;
    Lines[1].B = Edge_R;

    Lines[2].A = Edge_L;
    Lines[2].B = Edge_LL;

    Lines[3].A = Edge_R;
    Lines[3].B = Edge_RR;

    Lines[4].A = Edge_LL;
    Lines[4].B = Edge_LLL;

    Lines[5].A = Edge_RR;
    Lines[5].B = Edge_RRR;
}

void Find_Intersects_and_Make_Embanks()
{
    double X_Datum, Y_Datum;
    double DeltaX, DeltaY;
    line Hand_Line3;  // For Temperory Use in this function; This is a single line

    // Part1: Finds Left Intersect Point
    X_Datum = GroundL_X;
    Y_Datum = GroundL_Height;
    DeltaX = 0 - X_Datum;
    DeltaY = GL_Height - Y_Datum;
    Left_Handy_Slope = DeltaY / DeltaX;

    Hand_Line3.A.x = GroundL_X;
    Hand_Line3.A.y = GroundL_Height;

    if ( Canals[0].Need )
    {
        L_Embank.A.x = Canals[0].Lines[4].B.x;
        L_Embank.A.y = Canals[0].Lines[4].B.y;
        L_Embank.Slope = -1 * z;
    }
    else
    {
        L_Embank.A.x = Edge_LLL.x;
        L_Embank.A.y = Edge_LLL.y;
        L_Embank.Slope = z;
    }

    Intersects[0].x = (Left_Handy_Slope * Hand_Line3.A.x - Hand_Line3.A.y - L_Embank.Slope * L_Embank.A.x + L_Embank.A.y) / (Left_Handy_Slope - L_Embank.Slope);
    Intersects[0].y = L_Embank.Slope * (Intersects[0].x - L_Embank.A.x) + L_Embank.A.y;

    // Sets Left Embank
    L_Embank.B.x = Intersects[0].x;
    L_Embank.B.y = Intersects[0].y;

    // Prepare To Draw
    Lines[6] = L_Embank;

    // Part2: Finds Right Intersect Point
    X_Datum = 0;
    Y_Datum = GL_Height;
    DeltaX = GroundR_X - 0;
    DeltaY = GroundR_Height - GL_Height;
    Right_Handy_Slope = DeltaY / DeltaX;

    Hand_Line3.A.x = 0;
    Hand_Line3.A.y = GL_Height;

    if ( Canals[1].Need )
    {
        R_Embank.A.x = Canals[1].Lines[4].B.x;
        R_Embank.A.y = Canals[1].Lines[4].B.y;
        R_Embank.Slope = z;
    }
    else
    {
        R_Embank.A.x = Edge_RRR.x;
        R_Embank.A.y = Edge_RRR.y;
        R_Embank.Slope = -1 * z;
    }

    Intersects[1].x = (Right_Handy_Slope * Hand_Line3.A.x - Hand_Line3.A.y - R_Embank.Slope * R_Embank.A.x + R_Embank.A.y) / (Right_Handy_Slope - R_Embank.Slope);
    Intersects[1].y = R_Embank.Slope * (Intersects[1].x - R_Embank.A.x) + R_Embank.A.y;

    // Sets Left Embank
    R_Embank.B.x = Intersects[1].x;
    R_Embank.B.y = Intersects[1].y;

    // Prepare To Draw
    Lines[7] = R_Embank;
}

void Make_Hand_Line()
{
    // Declarations
    double X_Datum, Y_Datum;
    double DeltaX, DeltaY;
    double part;
    double Hand_Effect;

    // Part 1 - from Left to center in GL
    if (A4_Paper.Portrait_Landscape == 'L' && A4_Paper.Scale == 100)
    {
        X_Datum = -15;
        Y_Datum = GL_Height - 15 * Left_Handy_Slope;
    }
    else if (A4_Paper.Portrait_Landscape == 'P' && A4_Paper.Scale == 100)
    {
        X_Datum = -10.5;
        Y_Datum = GL_Height - 10.5 * Left_Handy_Slope;
    }
    else if (A4_Paper.Portrait_Landscape == 'L' && A4_Paper.Scale == 200)
    {
        X_Datum = -30;
        Y_Datum = GL_Height - 30 * Left_Handy_Slope;
    }
    else if (A4_Paper.Portrait_Landscape == 'P' && A4_Paper.Scale == 200)
    {
        X_Datum = -21;
        Y_Datum = GL_Height - 21 * Left_Handy_Slope;
    }

    if (Y_Datum < A4_Paper.Lines[1].A.y)
        X_Datum = ((Y_Datum = A4_Paper.Lines[1].A.y) - GL_Height) / Left_Handy_Slope;
    if (Y_Datum > A4_Paper.Lines[3].A.y)
        X_Datum = ((Y_Datum = A4_Paper.Lines[3].A.y) - GL_Height) / Left_Handy_Slope;

    DeltaX = 0 - X_Datum;
    DeltaY = GL_Height - Y_Datum;
    part = DeltaX / Hand_Line_Point_Count * 2;

    // This Part, will make a straight line by 100 small lines-Left
    for (int i = 0; i < (Hand_Line_Point_Count) / 2; ++i)
    {
        Hand_Line[i].A.x = X_Datum + i * part;
        Hand_Line[i].A.y = Y_Datum + i * Left_Handy_Slope * part;

        Hand_Line[i].B.x = X_Datum + (i + 1) * part;
        Hand_Line[i].B.y = Y_Datum + (i + 1) * Left_Handy_Slope * part;

        // Snapping Handy Line & Paper
        if (i == (Hand_Line_Point_Count) / 2 - 1)
            Hand_Line[i].B.y = GL_Height;
    }

    // Assumes Suitable Hand Effect
    double dbl;
    dbl = GL_Height - GroundL_Height;
    Hand_Effect = -.5 * (abs(dbl) - 10) + 10;

    // This Part's tries to make a straight line, HAANDYYY!!! Left
    int jump;
    srand(time(0));
    for (int i = .02 * Hand_Line_Point_Count / 2; i < .98 * Hand_Line_Point_Count / 2 ; ++i)
    {
        jump = rand() % 6;
        Hand_Line[i].B.y +=  9 * .001 * DeltaY * sin (5 * sin(jump));
        Hand_Line[i+1].A.y += 9 * .001 * DeltaY * sin (5 * sin(jump));
    }

    // Snapping Handy Line & Paper
    for (int i = 0; i < (Hand_Line_Point_Count) / 2; ++i)
        if (Hand_Line[i].A.x > Intersects[0].x && Hand_Line[i-1].A.x < Intersects[0].x)
        {
            Hand_Line[i-1].B = Intersects[0];
            Hand_Line[i].A = Intersects[0];
        }
    if (Canals[2].Need)
        for (int i = 0; i < (Hand_Line_Point_Count) / 2; ++i)
        {
            if (Hand_Line[i].B.x > Canals[2].Lines[2].B.x && Hand_Line[i].A.x < Canals[2].Lines[2].B.x)
                Hand_Line[i].B = Canals[2].Lines[2].B;
            if (Hand_Line[i].A.x > Canals[2].Lines[2].B.x && Hand_Line[i].B.x < Canals[2].Lines[0].A.x)
                Hand_Line[i].A = Hand_Line[i].B = Canals[2].Lines[2].B;
            if (Hand_Line[i].A.x < Canals[2].Lines[0].A.x && Hand_Line[i].B.x > Canals[2].Lines[0].A.x)
                Hand_Line[i].A = Canals[2].Lines[0].A;
        }

    // Part 2 - from center in GL to Right
    double X_Final, Y_Final;

    if (A4_Paper.Portrait_Landscape == 'L' && A4_Paper.Scale == 100)
    {
        X_Final = 15;
        Y_Final = GL_Height + 15 * Right_Handy_Slope;
    }
    if (A4_Paper.Portrait_Landscape == 'P' && A4_Paper.Scale == 100)
    {
        X_Final = 10.5;
        Y_Final = GL_Height + 10.5 * Right_Handy_Slope;
    }
    if (A4_Paper.Portrait_Landscape == 'L' && A4_Paper.Scale == 200)
    {
        X_Final = 30;
        Y_Final = GL_Height + 30 * Right_Handy_Slope;
    }
    if (A4_Paper.Portrait_Landscape == 'P' && A4_Paper.Scale == 200)
    {
        X_Final = 21;
        Y_Final = GL_Height + 21 * Right_Handy_Slope;
    }

    if (Y_Final < A4_Paper.Lines[1].A.y)
        X_Final = ((Y_Final = A4_Paper.Lines[1].A.y) - GL_Height) / Right_Handy_Slope;
    if (Y_Final > A4_Paper.Lines[3].A.y)
        X_Final = ((Y_Final = A4_Paper.Lines[3].A.y) - GL_Height) / Right_Handy_Slope;

    X_Datum = 0;
    Y_Datum = GL_Height;
    DeltaX =  X_Final - X_Datum;
    DeltaY = Y_Final - Y_Datum;
    part = DeltaX / Hand_Line_Point_Count * 2;

    // This Part, will make a straight line by 100 small lines-Right
    for (int i = 0; i < Hand_Line_Point_Count / 2; ++i)
    {
        Hand_Line2[i].A.x = X_Datum + i * part;
        Hand_Line2[i].A.y = Y_Datum + i * Right_Handy_Slope * part;

        Hand_Line2[i].B.x = X_Datum + (i + 1) * part;
        Hand_Line2[i].B.y = Y_Datum + (i + 1) * Right_Handy_Slope * part;

        // Snapping Handy Line & Paper
        if (i == (Hand_Line_Point_Count / 2) - 1)
            Hand_Line2[i].B.y = Y_Final;
    }

    // Assumes Suitable Hand Effect-Right
    dbl = GL_Height - GroundR_Height;
    Hand_Effect = -.5 * (abs(dbl) - 10) + 10;

    // This Part's tries to make a straight line, HAANDYYY!!!  Right
    srand(time(0));
    for (int i = .02 * Hand_Line_Point_Count / 2; i < .98 * Hand_Line_Point_Count / 2 ; ++i)
    {
        jump = rand() % 6;
        Hand_Line2[i].B.y += 6 * .001 * DeltaY * sin (5 * sin(jump));
        Hand_Line2[i+1].A.y += 6 * .001 * DeltaY * sin (5 * sin(jump));
    }

    // Snapping Handy Line & Paper
    for (int i = 0; i < Hand_Line_Point_Count / 2; ++i)
        if (Hand_Line2[i].A.x > Intersects[1].x && Hand_Line2[i-1].A.x < Intersects[1].x)
        {
            Hand_Line2[i-1].B = Intersects[1];
            Hand_Line2[i].A = Intersects[1];
        }
    if (Canals[3].Need)
        for (int i = 0; i < (Hand_Line_Point_Count) / 2; ++i)
        {
            if (Hand_Line2[i].A.x < Canals[3].Lines[0].A.x && Hand_Line2[i].B.x > Canals[3].Lines[0].A.x)
                Hand_Line2[i].B = Canals[3].Lines[0].A;
            if (Hand_Line2[i].A.x > Canals[3].Lines[0].A.x && Hand_Line2[i].B.x < Canals[3].Lines[2].B.x)
                Hand_Line2[i].A = Hand_Line2[i].B = Canals[3].Lines[2].B;
            if (Hand_Line2[i].A.x < Canals[3].Lines[2].B.x && Hand_Line2[i].B.x > Canals[3].Lines[2].B.x)
                Hand_Line2[i].A = Canals[3].Lines[2].B;
        }
}

void Canals_Need_Invest()
{
    // Compares Top Ground Level of Edge_LLL.y
    double m_Left, Ground_LLL_Height;
    m_Left = (GL_Height - GroundL_Height) / 10;
    Ground_LLL_Height = GL_Height + m_Left * Edge_LLL.x;
    if (Ground_LLL_Height > Edge_LLL.y)
        Canals[0].Need = true;

    double m_Right, Ground_RRR_Height;
    m_Right = (GroundR_Height - GL_Height) / 10;
    Ground_RRR_Height = GL_Height + m_Right * Edge_RRR.x;
    if (Ground_RRR_Height > Edge_RRR.y)
        Canals[1].Need = true;

    if (Canals[0].Need || Canals[1].Need)
        Make_Small_Canals();

    if (GroundL_Height > GL_Height && Canals[0].Need)
        Canals[2].Need = true;
    else
        Canals[2].Need = false;

    if (GroundR_Height > GL_Height && Canals[1].Need)
        Canals[3].Need = true;
    else
        Canals[3].Need = false;
}

void Make_Small_Canals()
{
    if (Canals[0].Need)
    {
        Canals[0].Lines[0].A = Edge_LLL;
        Canals[0].Lines[0].B = Canals[0].Lines[0].A;
        Canals[0].Lines[0].B.x = Canals[0].Lines[0].A.x - .2;

        Canals[0].Lines[1].A = Canals[0].Lines[0].B;
        Canals[0].Lines[1].B.x = Canals[0].Lines[1].A.x - .1;
        Canals[0].Lines[1].B.y = Canals[0].Lines[1].A.y - .3;

        Canals[0].Lines[2].A = Canals[0].Lines[1].B;
        Canals[0].Lines[2].B = Canals[0].Lines[2].A;
        Canals[0].Lines[2].B.x = Canals[0].Lines[2].A.x - .25;

        Canals[0].Lines[3].A = Canals[0].Lines[2].B;
        Canals[0].Lines[3].B.x = Canals[0].Lines[3].A.x - .1;
        Canals[0].Lines[3].B.y = Canals[0].Lines[3].A.y + .3;

        Canals[0].Lines[4].A = Canals[0].Lines[3].B;
        Canals[0].Lines[4].B = Canals[0].Lines[4].A;
        Canals[0].Lines[4].B.x = Canals[0].Lines[4].A.x - .2;
    }

    if (Canals[1].Need)
    {
        Canals[1].Lines[0].A = Edge_RRR;
        Canals[1].Lines[0].B = Canals[1].Lines[0].A;
        Canals[1].Lines[0].B.x = Canals[1].Lines[0].A.x + .2;

        Canals[1].Lines[1].A = Canals[1].Lines[0].B;
        Canals[1].Lines[1].B.x = Canals[1].Lines[1].A.x + .1;
        Canals[1].Lines[1].B.y = Canals[1].Lines[1].A.y - .3;

        Canals[1].Lines[2].A = Canals[1].Lines[1].B;
        Canals[1].Lines[2].B = Canals[1].Lines[2].A;
        Canals[1].Lines[2].B.x = Canals[1].Lines[2].A.x + .25;

        Canals[1].Lines[3].A = Canals[1].Lines[2].B;
        Canals[1].Lines[3].B.x = Canals[1].Lines[3].A.x + .1;
        Canals[1].Lines[3].B.y = Canals[1].Lines[3].A.y + .3;

        Canals[1].Lines[4].A = Canals[1].Lines[3].B;
        Canals[1].Lines[4].B = Canals[1].Lines[4].A;
        Canals[1].Lines[4].B.x = Canals[1].Lines[4].A.x + .2;
    }
}

void Make_Big_Canals()
{
    double Big_Canal_Height, Big_Canal_Wide;
    const float H1 = .3, H2 = .45, H3 = .6;
    const float W1 = .35, W2 = .55, W3 = .7;
    // Left Big Canal
    if (Canals[2].Need && Left_Handy_Slope > -0.2)
    {
        Big_Canal_Height = H1;
        Big_Canal_Wide = W1;
    }
    else if (Canals[2].Need && Left_Handy_Slope > -0.5)
    {
        Big_Canal_Height = H2;
        Big_Canal_Wide = W2;
    }
    else if (Canals[2].Need)
    {
        Big_Canal_Height = H3;
        Big_Canal_Wide = W3;
    }

    if (Canals[2].Need)
    {
        Canals[2].Lines[0].A.x = Intersects[0].x - .5;
        Canals[2].Lines[0].A.y = Intersects[0].y - .5 * Left_Handy_Slope;
        Canals[2].Lines[0].B.x = Canals[2].Lines[0].A.x - Big_Canal_Height / 2;
        Canals[2].Lines[0].B.y = Canals[2].Lines[0].A.y - Big_Canal_Height;

        Canals[2].Lines[1].A = Canals[2].Lines[0].B;
        Canals[2].Lines[1].B.x = Canals[2].Lines[1].A.x - Big_Canal_Wide;
        Canals[2].Lines[1].B.y = Canals[2].Lines[1].A.y;

        Canals[2].Lines[2].A = Canals[2].Lines[1].B;
        Canals[2].Lines[2].B.x = Canals[2].Lines[2].A.x - Big_Canal_Height;
        Canals[2].Lines[2].B.y = Canals[2].Lines[0].A.y - (Big_Canal_Wide + 1.5 * Big_Canal_Height) * Left_Handy_Slope;

        Canals[2].Lines[3] = Canals[2].Lines[4] = Canals[2].Lines[2];
    }

    // Right Big Canal
    if (Canals[3].Need && Right_Handy_Slope < 0.2)
    {
        Big_Canal_Height = H1;
        Big_Canal_Wide = W1;
    }
    else if (Canals[3].Need && Right_Handy_Slope < 0.5)
    {
        Big_Canal_Height = H2;
        Big_Canal_Wide = W2;
    }
    else if (Canals[3].Need)
    {
        Big_Canal_Height = H3;
        Big_Canal_Wide = W3;
    }

    if (Canals[3].Need)
    {
        Canals[3].Lines[0].A.x = Intersects[1].x + .5;
        Canals[3].Lines[0].A.y = Intersects[1].y + .5 * Right_Handy_Slope;
        Canals[3].Lines[0].B.x = Canals[3].Lines[0].A.x + Big_Canal_Height / 2;
        Canals[3].Lines[0].B.y = Canals[3].Lines[0].A.y - Big_Canal_Height;

        Canals[3].Lines[1].A = Canals[3].Lines[0].B;
        Canals[3].Lines[1].B.x = Canals[3].Lines[1].A.x + Big_Canal_Wide;
        Canals[3].Lines[1].B.y = Canals[3].Lines[1].A.y;

        Canals[3].Lines[2].A = Canals[3].Lines[1].B;
        Canals[3].Lines[2].B.x = Canals[3].Lines[2].A.x + Big_Canal_Height;
        Canals[3].Lines[2].B.y = Canals[3].Lines[0].A.y + (Big_Canal_Wide + 1.5 * Big_Canal_Height) * Right_Handy_Slope;

        Canals[3].Lines[3] = Canals[3].Lines[4] = Canals[3].Lines[2];
    }
}

void Calculate_Area()
{
    for (int j = 0; j < 2; ++j)
    {
        point Temp_Point;
        Points[0].x = PL.x;

        Points[1] = GL;
        if (j == 0)
        {
            Points[0].y = Lines[6].A.y;// PL.y-Asphalt_Thick;
            Points[2] = Lines[6].B;
            Points[3] = Lines[6].A;
        }
        else
        {
            Points[0].y = Lines[7].A.y;// PL.y-Asphalt_Thick;
            Points[2] = Lines[7].B;
            Points[3] = Lines[7].A;
        }
        //cout<<Points[0].x<<endl<<Points[0].y<<endl<<Points[1].x<<endl<<Points[1].y<<endl<<Points[2].x<<endl<<Points[2].y<<endl<<Points[3].x<<endl<<Points[3].y<<endl;
        if((Points[1].y >= Points[0].y && Points[2].y >= Points[0].y) || (Points[1].y <= Points[0].y && Points[2].y <= Points[0].y))
        {
            double a = sqrt(pow(Points[0].x-Points[1].x,2)+pow(Points[0].y-Points[1].y,2)); //01
            double b = sqrt(pow(Points[1].x-Points[2].x,2)+pow(Points[1].y-Points[2].y,2)); //12
            double c = sqrt(pow(Points[2].x-Points[3].x,2)+pow(Points[2].y-Points[3].y,2)); //23
            double d = sqrt(pow(Points[3].x-Points[0].x,2)+pow(Points[3].y-Points[0].y,2)); //30
            double e = sqrt(pow(Points[0].x-Points[2].x,2)+pow(Points[0].y-Points[2].y,2)); //13

            double p1=(a+b+e)/2;
            double p2=(c+d+e)/2;
            //cout<<a<<endl<<b<<endl<<c<<endl<<d<<endl<<e<<endl;
            if(Points[1].y >= Points[0].y && Points[2].y >= Points[0].y)
            {
                Cut_Area +=(sqrt(p1*(p1-a)*(p1-b)*(p1-e))+sqrt(p2*(p2-c)*(p2-d)*(p2-e)))- Lp_Slope * Lp_Wide - L_Slope * L_Wide - L_Wide * Asphalt_Thick;
                /*
                cout<<"Halate 1"<<endl;
                cout<<Cut_Area<<endl;
                */
            }
            else
            {
                Fill_Area +=(sqrt(p1*(p1-a)*(p1-b)*(p1-e))+sqrt(p2*(p2-c)*(p2-d)*(p2-e)));
                /*
                cout<<"Halate 2"<<endl;
                cout<<Fill_Area<<endl;
                */
            }
        }
        else
        {
            Temp_Point.x=Points[1].x+ (GL.y-Points[1].y)*(Points[1].x-Points[2].x)/(Points[1].y-Points[2].y);
            Temp_Point.y=PL.y;
            double a=sqrt(pow(Points[0].x-Points[1].x,2)+pow(Points[0].y-Points[1].y,2)); // 01
            double b=sqrt(pow(Temp_Point.x-Points[0].x,2)+pow(Temp_Point.y-Points[0].y,2));// 1 temp
            double c=sqrt(pow(Temp_Point.x-Points[1].x,2)+pow(Temp_Point.y-Points[1].y,2));// 2 temp
            double d=sqrt(pow(Points[2].x-Points[3].x,2)+pow(Points[2].y-Points[3].y,2)); // 23
            double e=sqrt(pow(Temp_Point.x-Points[2].x,2)+pow(Temp_Point.y-Points[2].y,2));// 2 temp
            double f=sqrt(pow(Temp_Point.x-Points[3].x,2)+pow(Temp_Point.y-Points[3].y,2));  // 3 temp
            double p1=(a+b+c)/2;
            double p2=(d+e+f)/2;
            if(Points[1].y < Points[0].y && Points[2].y > Points[0].y)
            {
                Cut_Area +=sqrt(p2*(p2-d)*(p2-e)*(p2-f));
                Fill_Area +=sqrt(p1*(p1-a)*(p1-b)*(p1-c));
                /*
                cout<<"Halate 3"<<endl;
                cout<<a<<endl<<b<<endl<<c<<endl<<d<<endl<<e<<endl<<f<<endl;
                cout<<Cut_Area<<endl;
                cout<<Fill_Area<<endl;
                */
            }
            else
            {
                Fill_Area +=sqrt(p2*(p2-d)*(p2-e)*(p2-f));
                Cut_Area +=sqrt(p1*(p1-a)*(p1-b)*(p1-c));
                /*
                cout<<"Halate 4"<<endl;
                cout<<a<<endl<<b<<endl<<c<<endl<<d<<endl<<e<<endl<<f<<endl;
                cout<<Cut_Area<<endl;
                cout<<Fill_Area<<endl;
                */
            }
        }
    }
}

void Make_Paper()
{
    double X_Max = 0, Y_Max = 0;
    double X_Min = 0, Y_Min = 0;
    double X_Center, Y_Center;

    if (GL_Height > PL_Height)
    {
        Y_Max = GL_Height;
        Y_Min = PL_Height;
    }
    else
    {
        Y_Max = PL_Height;
        Y_Min = GL_Height;
    }

    if (Ground_Points_Draw == 1)
    {
        X_Max = 10.8;
        X_Min = -10;
        if (GroundL_Height > GroundR_Height)
        {
            Y_Max = GroundL_Height;
            Y_Min = GroundR_Height;
        }
        else
        {
            Y_Max = GroundR_Height;
            Y_Min = GroundL_Height;
        }
    }

    for ( int i = 6 ; i < 8 ; ++i )  // 6 Line is used and 2 lines are Left and Right Embanks
    {                                // Only 2 Embank Lines are important,
        if (X_Max < Lines[i].A.x)
            X_Max = Lines[i].A.x;
        if (X_Max < Lines[i].B.x)
            X_Max = Lines[i].B.x;

        if (X_Min > Lines[i].A.x)
            X_Min = Lines[i].A.x;
        if (X_Min > Lines[i].B.x)
            X_Min = Lines[i].B.x;

        if (Y_Max < Lines[i].A.y)
            Y_Max = Lines[i].A.y;
        if (Y_Max < Lines[i].B.y)
            Y_Max = Lines[i].B.y;

        if (Y_Min > Lines[i].A.y)
            Y_Min = Lines[i].A.y;
        if (Y_Min > Lines[i].B.y)
            Y_Min = Lines[i].B.y;
    }

    if (Canals[2].Need)
    {
        if (Canals[2].Lines[2].B.x < X_Min)
            X_Min = Canals[2].Lines[2].B.x;
        if (Canals[2].Lines[2].B.y > Y_Max)
            Y_Max = Canals[2].Lines[2].B.y;
    }
    if (Canals[3].Need)
    {
        if (Canals[3].Lines[2].B.x > X_Max)
            X_Max = Canals[3].Lines[2].B.x;
        if (Canals[3].Lines[2].B.y > Y_Max)
            Y_Max = Canals[3].Lines[2].B.y;
    }
    // Selecting portrait or landscape and Scale
    if (((X_Max - 0) < 0.85 * 15) && ((0 - X_Min) < 0.9 * 15) && ((Y_Max - Y_Min) < 0.9 * 21))
    {
        A4_Paper.height = 21;
        A4_Paper.width = 30;
        A4_Paper.Scale = 100;
        A4_Paper.Portrait_Landscape = 'L';
    }
    else if (((X_Max - 0) < 0.9 * 10.5) && ((0 - X_Min) < 0.9 * 10.5) && ((Y_Max - Y_Min) < 0.9 * 30))
    {
        A4_Paper.height = 30;
        A4_Paper.width = 21;
        A4_Paper.Scale = 100;
        A4_Paper.Portrait_Landscape = 'P';
    }
    else if (((X_Max - 0) < 0.9 * 30) && ((0 - X_Min) < 0.9 * 30) && ((Y_Max - Y_Min) < 0.9 * 42))
    {
        A4_Paper.height = 42;
        A4_Paper.width = 60;
        A4_Paper.Scale = 200;
        A4_Paper.Portrait_Landscape = 'L';
    }
    else if (((X_Max - 0) < 0.9 * 21) && ((0 - X_Min) < 0.9 * 21) && ((Y_Max - Y_Min) > 0.9 * 60))
    {
        A4_Paper.height = 60;
        A4_Paper.width = 42;
        A4_Paper.Scale = 200;
        A4_Paper.Portrait_Landscape = 'P';
    }
    else if (1)
    {
        A4_Paper.height = 42;
        A4_Paper.width = 60;
        A4_Paper.Scale = 200;
        A4_Paper.Portrait_Landscape = 'L';
    }

    X_Center = 0;
    Y_Center = (Y_Max + Y_Min) / 2;

    // First Border
    A4_Paper.Lines[0].A.x = X_Center + A4_Paper.width / 2;
    A4_Paper.Lines[0].A.y = Y_Center + A4_Paper.height / 2;
    A4_Paper.Lines[0].B.x = X_Center + A4_Paper.width / 2;
    A4_Paper.Lines[0].B.y = Y_Center - A4_Paper.height / 2;

    A4_Paper.Lines[1].A = A4_Paper.Lines[0].B;
    A4_Paper.Lines[1].B.x = X_Center - A4_Paper.width / 2;
    A4_Paper.Lines[1].B.y = Y_Center - A4_Paper.height / 2;

    A4_Paper.Lines[2].A = A4_Paper.Lines[1].B;
    A4_Paper.Lines[2].B.x = X_Center - A4_Paper.width / 2;
    A4_Paper.Lines[2].B.y = Y_Center + A4_Paper.height / 2;

    A4_Paper.Lines[3].A = A4_Paper.Lines[2].B;
    A4_Paper.Lines[3].B = A4_Paper.Lines[0].A;

    // Second Border
    A4_Paper.Lines[4].A.x = 1.05 * A4_Paper.Lines[0].A.x;
    A4_Paper.Lines[4].A.y = A4_Paper.Lines[0].A.y + 0.05 * A4_Paper.Lines[0].A.x;
    A4_Paper.Lines[4].B.x = A4_Paper.Lines[4].A.x;
    A4_Paper.Lines[4].B.y = A4_Paper.Lines[0].B.y - 0.05 * A4_Paper.Lines[0].A.x;

    A4_Paper.Lines[5].A = A4_Paper.Lines[4].B;
    A4_Paper.Lines[5].B.x = - 1 * A4_Paper.Lines[4].A.x;
    A4_Paper.Lines[5].B.y = A4_Paper.Lines[4].B.y;

    A4_Paper.Lines[6].A = A4_Paper.Lines[5].B;
    A4_Paper.Lines[6].B.x = A4_Paper.Lines[5].B.x;
    A4_Paper.Lines[6].B.y = A4_Paper.Lines[4].A.y;

    A4_Paper.Lines[7].A = A4_Paper.Lines[6].B;
    A4_Paper.Lines[7].B.x = A4_Paper.Lines[4].A.x;
    A4_Paper.Lines[7].B.y = A4_Paper.Lines[4].A.y;

    // Center Line
    double L;
    L = A4_Paper.height / CL_Line_Number / 3;
    for (int i = 0; i < CL_Line_Number; ++i)
    {
        Center_Line[i].A.x = 0;
        Center_Line[i].B.x = 0;
    }
    for (int i = 0; i < CL_Line_Number; ++i)
    {
        Center_Line[i].A.y = A4_Paper.Lines[0].A.y - 3 * i * L;
        Center_Line[i].B.y = Center_Line[i].A.y - L;
    }

}
void Make_Report()
{
    fstream PROP_Out ;
    PROP_Out.open("p",ios::out);

    PROP_Out << "Station:" << Station_Name << endl
             << "GL:" << GL_Height << endl
             << "PL:" << PL_Height << endl
             << "LeftX:" << GroundL_X << endl
             << "Left Ground Height:" << GroundL_Height << endl
             << "RightX:" << GroundR_X << endl
             << "Right Ground Height:" << GroundR_Height << endl
             << "Left Width:" << L_Wide << endl
             << "Left Slope:" << L_Slope * 100 << endl
             << "Right Width:" << R_Wide << endl
             << "Right Slope:" << R_Slope * 100 << endl
             << "Left Shoulder Width:" << Lp_Wide << endl
             << "Left Shoulder Slope:" << Lp_Slope << endl
             << "Right Shoulder Width:" << Rp_Wide << endl
             << "Right Shoulder Slope:" << Rp_Slope << endl
             << "Asphalt Thick:" << Asphalt_Thick << endl
             << "Embankment Slope:" << z << endl
             << "Cut Area:" << Cut_Area << endl
             << "Fill Area:" << Fill_Area;
}

void MakeDXF()
{
    int iLineCode = 0;
    int iColor = 0;
    fstream DXF_Out ;
    int Fit_Scale = 1;

    if (A4_Paper.Scale == 100)
        Fit_Scale = 1;
    else
        Fit_Scale = 2;

    DXF_Out.open("t",ios::out);

    DXF_Out << "  2" << endl
            << "HEADER" << endl
            << "  0" << endl
            << "SECTION" << endl
            << "  2" << endl
            << "ENTITIES" << endl ;

    for ( int i = 0 ; i < 8 ; ++i )  // 6 Line is used here and 2 lines are Left and Right Embarks
    {
        // Road's Major Line and 2 Embarks
        DXF_Out << "  0" << endl
                << "LINE" << endl
                << "  5" << endl
                << ++iLineCode << endl  //Line Code
                << "  8" << endl
                << "0" << endl
                << " 62" << endl;    //Line Color
        if (i < 4)
            iColor = 1;
        else if (i < 6)
            iColor = 45;
        else if (i < 8)
            iColor = 3;
        DXF_Out << "  " << iColor << endl
                << " 10" << endl
                << Lines[i].A.x << endl
                << " 20" << endl
                << Lines[i].A.y << endl
                << " 30" << endl
                << "0.0" << endl
                << " 11" << endl
                << Lines[i].B.x << endl
                << " 21" << endl
                << Lines[i].B.y << endl
                << " 31" << endl
                << "0.0" << endl;
    }
    for ( int i = 0 ; i < Hand_Line_Point_Count / 2 ; ++i )
        // Hand Line1
        DXF_Out << "  0" << endl
                << "LINE" << endl
                << "  5" << endl
                << ++iLineCode << endl  //Line Code
                << "  8" << endl
                << "0" << endl
                << " 62" << endl    //Line Color
                << " 34" << endl
                << " 10" << endl
                << Hand_Line[i].A.x << endl
                << " 20" << endl
                << Hand_Line[i].A.y << endl
                << " 30" << endl
                << "0.0" << endl
                << " 11" << endl
                << Hand_Line[i].B.x << endl
                << " 21" << endl
                << Hand_Line[i].B.y << endl
                << " 31" << endl
                << "0.0" << endl;

    for ( int i = 0 ; i < Hand_Line_Point_Count / 2 ; ++i )
        // Hand Line2
        DXF_Out << "  0" << endl
                << "LINE" << endl
                << "  5" << endl
                << ++iLineCode << endl  //Line Code
                << "  8" << endl
                << "0" << endl
                << " 62" << endl    //Line Color
                << "  34" << endl
                << " 10" << endl
                << Hand_Line2[i].A.x << endl
                << " 20" << endl
                << Hand_Line2[i].A.y << endl
                << " 30" << endl
                << "0.0" << endl
                << " 11" << endl
                << Hand_Line2[i].B.x << endl
                << " 21" << endl
                << Hand_Line2[i].B.y << endl
                << " 31" << endl
                << "0.0" << endl;

    for ( int i = 0 ; i < 4 ; ++i )
        if (Canals[i].Need)
            for ( int j = 0 ; j < 5 ; ++j )
        // Canal Line
        DXF_Out << "  0" << endl
                << "LINE" << endl
                << "  5" << endl
                << ++iLineCode << endl  //Line Code
                << "  8" << endl
                << "0" << endl
                << " 62" << endl    //Line Color
                << "  152" << endl
                << " 10" << endl
                << Canals[i].Lines[j].A.x << endl
                << " 20" << endl
                << Canals[i].Lines[j].A.y << endl
                << " 30" << endl
                << "0.0" << endl
                << " 11" << endl
                << Canals[i].Lines[j].B.x << endl
                << " 21" << endl
                << Canals[i].Lines[j].B.y << endl
                << " 31" << endl
                << "0.0" << endl;

    for ( int i = 0 ; i < 8 ; ++i )
        // Paper Line
        DXF_Out << "  0" << endl
                << "LINE" << endl
                << "  5" << endl
                << ++iLineCode << endl  //Line Code
                << "  8" << endl
                << "0" << endl
                //<< " 62" << endl    //Line Color
                //<< "  152" << endl
                << " 10" << endl
                << A4_Paper.Lines[i].A.x << endl
                << " 20" << endl
                << A4_Paper.Lines[i].A.y << endl
                << " 30" << endl
                << "0.0" << endl
                << " 11" << endl
                << A4_Paper.Lines[i].B.x << endl
                << " 21" << endl
                << A4_Paper.Lines[i].B.y << endl
                << " 31" << endl
                << "0.0" << endl;

    for ( int i = 0 ; i < CL_Line_Number ; ++i )
         // Center Line Line
        DXF_Out << "  0" << endl
                << "LINE" << endl
                << "  5" << endl
                << ++iLineCode << endl  //Line Code
                << "  8" << endl
                << "0" << endl
                //<< " 62" << endl    //Line Color
                //<< "  152" << endl
                << " 10" << endl
                << Center_Line[i].A.x << endl
                << " 20" << endl
                << Center_Line[i].A.y << endl
                << " 30" << endl
                << "0.0" << endl
                << " 11" << endl
                << Center_Line[i].B.x << endl
                << " 21" << endl
                << Center_Line[i].B.y << endl
                << " 31" << endl
                << "0.0" << endl;

                // CL Text
        DXF_Out << "  0" << endl
                << "TEXT" << endl
                << "  8" << endl
                << "0" << endl
                << "  10" << endl
                << -.3 * Fit_Scale << endl
                << "  20" << endl
                << "                "<< A4_Paper.Lines[1].A.y + .3 * Fit_Scale << endl
                << "  40" << endl
                << .4 * Fit_Scale << endl  //Size
                << "  50" << endl
                << "0" << endl //Degree
                << "  1" << endl
                << "CL" << endl;

                // Area Fill: Text
        DXF_Out << "  0" << endl
                << "TEXT" << endl
                << "  8" << endl
                << "0" << endl
                << "  10" << endl
                << "                "<< A4_Paper.Lines[1].B.x + .3 * Fit_Scale << endl
                << "  20" << endl
                << "                "<< A4_Paper.Lines[1].B.y + 1.1 * Fit_Scale << endl
                << "  40" << endl
                << .4 * Fit_Scale << endl  //Size
                << "  50" << endl
                << "0" << endl //Degree
                << "  1" << endl;
        if (Do_Calculate_Area)
            DXF_Out << "Area Fill: " << Fill_Area << endl;
        else
            DXF_Out << "Area Fill: " << endl;

                // Area Cut: Text
        DXF_Out << "  0" << endl
                << "TEXT" << endl
                << "  8" << endl
                << "0" << endl
                << "  10" << endl
                << "                "<< A4_Paper.Lines[1].B.x + .3 * Fit_Scale << endl
                << "  20" << endl
                << "                "<< A4_Paper.Lines[1].B.y + 1.8 * Fit_Scale << endl
                << "  40" << endl
                << .4 * Fit_Scale << endl  //Size
                << "  50" << endl
                << "0" << endl //Degree
                << "  1" << endl;
        if ( Do_Calculate_Area)
            DXF_Out << "Area Cut: " << Cut_Area << endl;
        else
            DXF_Out << "Area Cut: " << endl;

            // Sc: Text
        DXF_Out << "  0" << endl
                << "TEXT" << endl
                << "  8" << endl
                << "0" << endl
                //<< " 62" << endl
                //<< "  " << PointColor << endl
                << "  10" << endl
                << "                "<< A4_Paper.Lines[1].B.x + .3 * Fit_Scale << endl
                << "  20" << endl
                << "                "<< A4_Paper.Lines[1].A.y + .3 * Fit_Scale << endl
                << "  40" << endl
                << .4 * Fit_Scale << endl  //Size
                << "  50" << endl
                << "0" << endl; //Degree
            if (A4_Paper.Scale == 100)
        DXF_Out << "  1" << endl
                << "Sc: 1:100" << endl;
            else if ( A4_Paper.Scale == 200)
        DXF_Out << "  1" << endl
                << "Sc: 1:200" << endl;

                // Station Name
        DXF_Out << "  0" << endl
                << "TEXT" << endl
                << "  8" << endl
                << "0" << endl
                //<< " 62" << endl
                //<< "  " << PointColor << endl
                << "  10" << endl
                << "                "<< A4_Paper.Lines[0].A.x - 4.5 * Fit_Scale << endl
                << "  20" << endl
                << "                "<< A4_Paper.Lines[0].A.y - .8 * Fit_Scale << endl
                << "  40" << endl
                << .4 * Fit_Scale << endl  //Size
                << "  50" << endl
                << "0" << endl //Degree
                << "  1" << endl
                << "Station: " << Station_Name << endl;

                // Program Name
        DXF_Out << "  0" << endl
                << "TEXT" << endl
                << "  8" << endl
                << "0" << endl
                //<< " 62" << endl
                //<< "  " << PointColor << endl
                << "  10" << endl
                << "                "<< A4_Paper.Lines[0].B.x - 8.4 * Fit_Scale << endl
                << "  20" << endl
                << "                "<< A4_Paper.Lines[0].B.y + .4 * Fit_Scale << endl
                << "  40" << endl
                << .35 * Fit_Scale << endl  //Size
                << "  50" << endl
                << "0" << endl //Degree
                << "  1" << endl
                << "proofLat AutoCAD Drawer" << endl;

                //GL Point & Text
        DXF_Out << "  0" << endl
                << "POLYLINE" << endl
                << "  8" << endl
                << "Point" << endl
                << " 62" << endl
                << "  0" << endl
                << " 66" << endl
                << "  1" << endl
                << " 10" << endl
                << "  0.0" << endl
                << " 20" << endl
                << "  0.0" << endl
                << " 70" << endl
                << "  1" << endl
                << " 40" << endl
                << "  0.141421" << endl
                << " 41" << endl
                << "  0.141421" << endl
                << " 0" << endl
                << "VERTEX" << endl
                << "  8" << endl
                << "Point" << endl
                << " 10" << endl
                << "                "<< "-.03" << endl
                << " 20" << endl
                << "                "<< GL_Height + .1 << endl
                << " 42" << endl
                << "  1.0" << endl
                << "  0" << endl
                << "VERTEX" << endl
                << "  8" << endl
                << "Point" << endl
                << " 10" << endl
                << "                "<< "0" << endl
                << " 20" << endl
                << "                "<< GL_Height << endl
                << " 42" << endl
                << "  1.0" << endl
                << "  0" << endl
                << "SEQEND" << endl
                << "  8" << endl
                << "Point" << endl
                << "  0" << endl
                << "TEXT" << endl
                << "  8" << endl
                << "0" << endl
                << " 62" << endl
                << "  0" << endl
                << "  10" << endl
                << "  .2"<< endl
                << "  20" << endl
                << "                "<< GL_Height << endl
                << "  40" << endl
                << .2 * Fit_Scale << endl
                << "  1" << endl
                << "GL:" << GL_Height << endl;

                //PL Point & Text
        DXF_Out << "  0" << endl
                << "POLYLINE" << endl
                << "  8" << endl
                << "Point" << endl
                << " 62" << endl
                << "  0" << endl
                << " 66" << endl
                << "  1" << endl
                << " 10" << endl
                << "  0.0" << endl
                << " 20" << endl
                << "  0.0" << endl
                << " 70" << endl
                << "  1" << endl
                << " 40" << endl
                << "  0.141421" << endl
                << " 41" << endl
                << "  0.141421" << endl
                << " 0" << endl
                << "VERTEX" << endl
                << "  8" << endl
                << "Point" << endl
                << " 10" << endl
                << "                "<< "-.03" << endl
                << " 20" << endl
                << "                "<< PL_Height + .1 << endl
                << " 42" << endl
                << "  1.0" << endl
                << "  0" << endl
                << "VERTEX" << endl
                << "  8" << endl
                << "Point" << endl
                << " 10" << endl
                << "                "<< "0" << endl
                << " 20" << endl
                << "                "<< PL_Height << endl
                << " 42" << endl
                << "  1.0" << endl
                << "  0" << endl
                << "SEQEND" << endl
                << "  8" << endl
                << "Point" << endl
                << "  0" << endl
                << "TEXT" << endl
                << "  8" << endl
                << "0" << endl
                << " 62" << endl
                << "  0" << endl
                << "  10" << endl
                << "  .2"<< endl
                << "  20" << endl
                << "                "<< PL_Height << endl
                << "  40" << endl
                << .2 * Fit_Scale << endl
                << "  1" << endl
                << "PL:" << PL_Height << endl;

    if (Ground_Points_Draw)
    {
                //Ground Left Point & Text
        DXF_Out << "  0" << endl
                << "POLYLINE" << endl
                << "  8" << endl
                << "Point" << endl
                << " 62" << endl
                << "  0" << endl
                << " 66" << endl
                << "  1" << endl
                << " 10" << endl
                << "  0.0" << endl
                << " 20" << endl
                << "  0.0" << endl
                << " 70" << endl
                << "  1" << endl
                << " 40" << endl
                << "  0.141421" << endl
                << " 41" << endl
                << "  0.141421" << endl
                << " 0" << endl
                << "VERTEX" << endl
                << "  8" << endl
                << "Point" << endl
                << " 10" << endl
                << "                "<< "-10" << endl
                << " 20" << endl
                << "                "<< GroundL_Height + .1 << endl
                << " 42" << endl
                << "  1.0" << endl
                << "  0" << endl
                << "VERTEX" << endl
                << "  8" << endl
                << "Point" << endl
                << " 10" << endl
                << "                "<< "-10" << endl
                << " 20" << endl
                << "                "<< GroundL_Height << endl
                << " 42" << endl
                << "  1.0" << endl
                << "  0" << endl
                << "SEQEND" << endl
                << "  8" << endl
                << "Point" << endl
                << "  0" << endl
                << "TEXT" << endl
                << "  8" << endl
                << "0" << endl
                << " 62" << endl
                << "  0" << endl
                << "  10" << endl
                << "  -9.8"<< endl
                << "  20" << endl
                << "                "<< GroundL_Height << endl
                << "  40" << endl
                << .2 * Fit_Scale << endl
                << "  1" << endl
                << GroundL_Height << endl

                //ground Right Point & Text
                << "  0" << endl
                << "POLYLINE" << endl
                << "  8" << endl
                << "Point" << endl
                << " 62" << endl
                << "  0" << endl
                << " 66" << endl
                << "  1" << endl
                << " 10" << endl
                << "  0.0" << endl
                << " 20" << endl
                << "  0.0" << endl
                << " 70" << endl
                << "  1" << endl
                << " 40" << endl
                << "  0.141421" << endl
                << " 41" << endl
                << "  0.141421" << endl
                << " 0" << endl
                << "VERTEX" << endl
                << "  8" << endl
                << "Point" << endl
                << " 10" << endl
                << "                "<< "10" << endl
                << " 20" << endl
                << "                "<< GroundR_Height + .1 << endl
                << " 42" << endl
                << "  1.0" << endl
                << "  0" << endl
                << "VERTEX" << endl
                << "  8" << endl
                << "Point" << endl
                << " 10" << endl
                << "                "<< "10" << endl
                << " 20" << endl
                << "                "<< GroundR_Height << endl
                << " 42" << endl
                << "  1.0" << endl
                << "  0" << endl
                << "SEQEND" << endl
                << "  8" << endl
                << "Point" << endl
                << "  0" << endl
                << "TEXT" << endl
                << "  8" << endl
                << "0" << endl
                << " 62" << endl
                << "  0" << endl
                << "  10" << endl
                << "  10.2"<< endl
                << "  20" << endl
                << "                "<< GroundR_Height << endl
                << "  40" << endl
                << .2 * Fit_Scale << endl
                << "  1" << endl
                << GroundR_Height << endl;
    }

    DXF_Out << "  0" << endl
            << "ENDSEC" << endl
            << "  0" << endl
            << "EOF" ;
    DXF_Out.close() ;
}
