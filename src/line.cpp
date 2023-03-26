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


struct line_equation { float a, b, c; };
static inline line_equation calculate_line_equation(const line &line) {
    float a = line.x0.y() - line.x1.y();
    float b = line.x1.x() - line.x0.x();
    float c = (line.x0.x() - line.x1.x()) * line.x0.y() + (line.x1.y() - line.x0.y()) * line.x0.x();

    return { a, b, c };
}

bool line::intersect_circle(const circle& circle, std::vector<math::vec2> &points) const {
    constexpr double eps = std::numeric_limits<double>::epsilon();

    line relative = { x0 - circle.center, x1 - circle.center };

    float r = circle.radius;
    auto [a, b, c] = calculate_line_equation(relative);

    float x0 = - a * c / (pow(a, 2) + pow(b, 2)), y0 = - b * c / (pow(a, 2) + pow(b, 2));
    if (pow(c, 2) > pow(r, 2) * (pow(a, 2) + pow(b, 2)) + eps)
        return false;

    if (abs(pow(c, 2) - pow(r, 2) * (pow(a, 2) + pow(b, 2))) < eps) {
        points.emplace_back(x0, y0);
        return true;
    }

    float d = pow(r, 2) - pow(c, 2) / (pow(a, 2) + pow(b, 2));
    float mult = sqrt (d / (pow(a, 2) + pow(b, 2)));

    math::vec2 intersections[] = {
        { x0 + b * mult, y0 - a * mult },
        { x0 - b * mult, y0 + a * mult }
    };

    for (auto &&intersection: intersections)
        points.emplace_back(intersection + circle.center);

    return true;
}


math::vec2 line::project(math::vec2 point) const noexcept {
    math::vec2 direction = (x1 - x0).normalize();
    return direction * direction.dot(point - x0) + x0;
}


static void draw_continuation_dashed(gl::drawing_manager mgr, math::vec2 from, math::vec2 to) {

    math::vec2 direction = (from - to).normalize();

    constexpr double fill_len = 0.05;
    constexpr double frag_len = 0.02 + fill_len;
    for (int i = 0; ; ++ i) {
        math::vec2 starting_point = direction * frag_len * i + from;
        if (!mgr.screen().contains(starting_point))
            break;

        mgr.draw_line(starting_point, starting_point + direction * fill_len);
    }
}

void line::draw(gl::drawing_manager mgr) const {
    // mgr.set_color({ 0.9f, 0.2f, 0.3f });
    math::vec3 stroke_color = mgr.stroke_color();

    mgr.set_outer_color(mgr.fill_color());
    mgr.draw_line(x0, x1);

    // mgr.set_stroke_color({ 0.99f, 0.59f, 0.39f });

    mgr.set_outer_color(stroke_color);
    draw_continuation_dashed(mgr, x1, x0);
    draw_continuation_dashed(mgr, x0, x1);
}
