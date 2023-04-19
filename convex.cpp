#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <list>

using namespace std;

class Point {
public:
    const int x;
    const int y;

    Point(int x_given, int y_given): x(x_given), y(y_given) {}

    bool operator <(const Point& b) {
        if (x < b.x) return true;
        else if (x == b.x && y < b.y) return true;
        else return false;
    }
};

bool compare(Point* a, Point* b) {
    return *a < *b;
}

// Is the angle between AB and BC is counter clockwise - take determinant
// If greater than zero it is counter clockwise
double orientation(Point* a, Point* b, Point* c) {
    double ax = (double) a->x, ay = (double) a->y, bx = (double) b->x, by = (double) b->y, cx = (double) c->x, cy = (double) c->y;
    double det = (bx - ax) * (cy - by) - (by - ay) * (cx - bx);
    return det;
}

vector<Point*> merge(vector<Point*> left, vector<Point*> right) {
    int d = left.size();
    int k = right.size();
    vector<Point*> hull;
    
    // If there is just one element orientation will be always 0 -> infinite loop
    if (d == 1 && k == 2) {
        hull.push_back(left[0]);
        if (orientation(left[0], right[0], right[1]) > 0) {
            hull.push_back(right[0]);
            hull.push_back(right[1]);
        }
        else {
            hull.push_back(right[1]);
            hull.push_back(right[0]);
        }
        return hull;
    }

    int l = 0, r = 0;
    int left_rightmost, right_leftmost;
    bool done;
    
    // find rightmost in left
    for (int i = 0; i < d; i++) {
        if (left[i]->x > left[l]->x) {
            l = i;
        }
    }

    // find leftmost in right
    for (int i = 0; i < k; i++) {
        if (right[i]->x < right[r]->x) {
            r = i;
        }
    }

    left_rightmost = l;
    right_leftmost = r;
    
    // Find upper tangent
    done = false;
    while (!done) {
        done = true;
        while (orientation(right[r], left[l], left[(l+1) % d]) <= 0 && d != 1) {
            l = (l+1) % d;
        }
        
        while (orientation(left[l], right[r], right[(r+k-1) % k]) >= 0 && k != 1) {
            r = (r-1+k) % k;
            done = false;
        }
    }

    int upper_l = l, upper_r = r;

    // Find lower tangent
    l = left_rightmost;
    r = right_leftmost;
    done = false;
    while (!done) {
        done = true;
        while (orientation(left[l], right[r], right[(r+1) % k]) <= 0 && k != 1) {
            r = (r+1) % k;
        }
        
        while (orientation(right[r], left[l], left[(l+d-1) % d]) >= 0 && d != 1) {
            l = (l+d-1) % d;
            done = false;
        }
    }

    int lower_l = l, lower_r = r;
    
    int i;
    
    // Merge from lower left to upper left
    i = upper_l;
    hull.push_back(left[upper_l]);
    while (i != lower_l) {
        i = (i+1) % d;
        hull.push_back(left[i]);
    }
    
    // Merge from upper right to lower left
    i = lower_r;
    hull.push_back(right[lower_r]);
    while (i != upper_r) {
        i = (i+1) % k;
        hull.push_back(right[i]);
    }
    
    return hull;
}

vector<Point*> CH(vector<Point*> points) {
    int n = points.size();
    if (n < 3) {
        return points;
    }

    vector<Point*> left;
    vector<Point*> right;
    int d = (int) (n/2);

    for(int i = 0; i < d; i++) {
        left.push_back(points[i]);
    }

    for(int i = d; i < n; i++) {
        right.push_back(points[i]);
    }

    left = CH(left);
    right = CH(right);

    return merge(left, right);
}

void print(vector<Point*> ch, Point* leftmost) {
    ofstream output_file("convex.txt");
    
    int n = ch.size();
    output_file << n << endl;
    
    // find leftmost index
    int i = 0;
    while (ch[i] != leftmost) {
        i = (i+1) % n;
    }
    
    output_file << "(" << ch[i]->x << "," << ch[i]->y << ")";
    i = (i+1) % n;

    // Print starting from leftmost
    while (ch[i] != leftmost) {
        output_file << "(" << ch[i]->x << "," << ch[i]->y << ")";
        i = (i+1) % n;
    }
    output_file << "(" << ch[i]->x << "," << ch[i]->y << ")";
    output_file.close();
}

int main(int argc, char* argv[]){
    // Read input file
    ifstream input_file(argv[1]);
    
    // Number of points
    int n;
    input_file >> n;
    
    // points' information
    vector<Point*> points;
    
    int x, y;
    for(int i = 0; i < n; i++) {
        input_file >> x >> y;
        points.push_back(new Point(x, y));
    }

    sort(points.begin(), points.end(), compare);

    print(CH(points), points[0]);

    exit(0);
}