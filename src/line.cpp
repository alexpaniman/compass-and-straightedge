#include "line.h"

bool line::intersect_line(const line& line, std::vector<math::vec2> &points) const {
    constexpr float eps = std::numeric_limits<float>::epsilon();

    float div = (line.x1.y() - line.x0.y()) * (x1.x() - x0.x())
        - (line.x1.x() - line.x0.x()) * (x1.y() - x0.y());

    if (fabsf(div) >= eps) {
        float x = ((line.x0.x() - line.x1.x()) *
            (x0.x() * x1.y() - x0.y() * x1.x()) - (x0.x() - x1.x()) *
            (line.x0.x() * line.x1.y() - line.x0.y() * line.x1.x())) / div;

        float y = ((line.x0.y() - line.x1.y()) *
            (x0.x() * x1.y() - x0.y() * x1.x()) - (x0.y() - x1.y()) *
            (line.x0.x() * line.x1.y() - line.x0.y() * line.x1.x())) / div;

        points.emplace_back(x, y);
        return true;
    }

    return false;
}


struct line_equation { double a, b, c; };
static inline line_equation calculate_line_equation(const line &line) {
    double a = line.x0.y() - line.x1.y();
    double b = line.x1.x() - line.x0.x();
    double c = (line.x0.x() - line.x1.x()) * line.x0.y() + (line.x1.y() - line.x0.y()) * line.x0.x();

    return { a, b, c };
}

bool line::intersect_circle(const circle& circle, std::vector<math::vec2> &points) const {
    constexpr double eps = std::numeric_limits<double>::epsilon();

    double r = circle.radius;
    auto [a, b, c] = calculate_line_equation(*this);

    double x0 = - a * c / (pow(a, 2) + pow(b, 2)), y0 = - b * c / (a * a + b * b);
    if (pow(c, 2) > pow(r, 2) * (pow(a, 2) + pow(b, 2)) + eps)
        return false;

    if (abs(pow(c, 2) - pow(r, 2) * (pow(a, 2) + pow(b, 2))) < eps) {
        points.emplace_back(x0, y0);
        return true;
    }

    double d = pow(r, 2) - pow(c, 2) / (pow(a, 2) + pow(b, 2));
    double mult = sqrt (d / (pow(a, 2) + pow(b, 2)));

    points.emplace_back(x0 + b * mult, y0 - a * mult);
    points.emplace_back(x0 - b * mult, y0 + a * mult);

    return true;
}


static inline bool within_view(math::vec2 point) {
    return point.x() >= -1.0f && point.y() >= -1.0f && point.x() <= 1.0f && point.y() <= 1.0f;
}

static void draw_continuation_dashed(gl::drawing_manager mgr, math::vec2 from, math::vec2 to) {
    math::vec2 direction = (from - to).normalize();

    constexpr double fill_len = 0.05;
    constexpr double frag_len = 0.02 + fill_len;
    for (int i = 0; ; ++ i) {
        math::vec2 starting_point = direction * frag_len * i + from;
        if (!within_view(starting_point))
            break;

        mgr.draw_line(starting_point, starting_point + direction * fill_len);
    }
}

void line::draw(gl::drawing_manager mgr) const {
    mgr.set_color({ 0.7f, 0.7f, 0.2f });
    mgr.draw_line(x0, x1);

    mgr.set_color({ 0.4f, 0.2f, 0.8f });
    draw_continuation_dashed(mgr, x1, x0);
    draw_continuation_dashed(mgr, x0, x1);
}
