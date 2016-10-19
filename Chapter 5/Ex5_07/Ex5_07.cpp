// Ex5_07.cpp
// Applying STL set algorithms
#include <iostream>                              // For standard streams
#include <string>                                // For string class
#include <map>                                   // For map container
#include <set>                                   // For set container
#include <vector>                                // For vector container
#include <random>                                // For random number generation
#include <algorithm>                             // For for_each(), count_if()
#include "Student.h"
#include "List_Course.h"

using std::string;
using Distribution = std::uniform_int_distribution<size_t>;
using Subject = string;                          // A course subject
using Subjects = std::vector<Subject>;           // A vector of subjects
using Group = std::set<Student>;                 // A student group for a subject
using Students = std::vector<Student>;           // All the students
using Course = std::pair<Subject, Group>;        // A pair representing a course
using Courses = std::map<Subject, Group>;        // The container for courses

static std::default_random_engine gen_value;

// Creates a vector of students
Students create_students()
{
  Students students;
  string first_names[] {"Ann", "Jim", "Eve", "Dan", "Ted"};
  string second_names[] {"Smith", "Jones", "Howe", "Watt", "Beck"};

  for(const auto& first : first_names)
    for(const auto& second : second_names)
    {
      students.emplace_back(first, second);
    }

  return students;
}

// Create a group of students for a subject
Group make_group(const Students& students, size_t group_size, const Distribution& choose_student)
{
  Group group;                                             // The group of students for a subject

  // Select students for the subject group
  size_t count {};                                         // No. of students in the group
  std::pair<Group::iterator, bool> pr;                     // Stores return from inserting a student

  // Insert a random student into the group until there are group_size students in it
  while(count < group_size)
  {  // Increment count for a successful insertion...
    if(group.insert(students[choose_student(gen_value)]).second) ++count;
  }
  return group;
}

int main()
{
  Students students = create_students();
  Subjects subjects {"Biology", "Physics", "Chemistry", "Mathematics", "Astronomy",
    "Drama", "Politics", "Philosophy", "Economics"};
  Courses courses;                                         // All the courses with subject keys


  size_t min_subjects {4};                                 // Minimum number of Subjects per student
  size_t min_group {min_subjects};                         // Minimum no. of students per course
  size_t max_group {(students.size()*min_subjects) / subjects.size()};  // Maximum initial students per course

  // Create groups of students for each subject
  Distribution group_size {min_group, max_group};          // Distribution for students per course
  Distribution choose_student {0, students.size() - 1};    // Random student selector
  for(const auto& subject : subjects)
    courses.emplace(subject, make_group(students, group_size(gen_value), choose_student));

  Distribution choose_course {0, subjects.size() - 1};     // Random course selector

  // Every student must attend a minimum number of Subjects...
  // ...but students being students we must check...
  for(const auto& student : students)
  { // Verify the minimum number of Subjects has been met

    // Count how many Subjects the student is on
    size_t course_count = std::count_if(std::begin(courses), std::end(courses), [&student](const Course& course)
    {  return course.second.count(student); });
    if(course_count >= min_subjects) continue;             // On to the next student

    // Minimum no. of Subjects not signed up for
    size_t additional {min_subjects - course_count};       // Additional no. of Subjects needed
    if(!course_count)                                      // If none have been chosen...
      std::cout << student << " is work-shy, having signed up for NO Subjects!\n";
    else                                                   // Some have - but E for effort
      std::cout << student << " is only signed up for " << course_count << " Subjects!\n";

    std::cout << "Registering " << student << " for " << additional
      << " more course" << (additional > 1 ? "s" : "") << ".\n\n";

    // Register for additional Subjects up to the minimum
    while(course_count < min_subjects)
      if((courses.find(subjects[choose_course(gen_value)])->second.insert(student)).second) ++course_count;
  }

  // Output the students attending each course
  std::for_each(std::begin(courses), std::end(courses), List_Course());
  std::cout << std::endl;

  // List students studying physics but not maths...
  auto physics = courses.find("Physics");
  auto maths = courses.find("Mathematics");
  if(physics == std::end(courses) || maths == std::end(courses)) throw std::invalid_argument {"Invalid course name."};
  std::cout << "\nStudents studying physics but not maths are:\n";
  std::set_difference(std::begin(physics->second), std::end(physics->second),
                      std::begin(maths->second), std::end(maths->second),
                      std::ostream_iterator < Student > {std::cout, "  "});
  std::cout << std::endl;

  // List students studying physics and maths...
  std::vector<Student> phys_and_math;
  std::cout << "\nStudents studying physics and maths are:\n";
  std::set_intersection(std::begin(physics->second), std::end(physics->second),
    std::begin(maths->second), std::end(maths->second),
    std::back_inserter(phys_and_math));
  std::copy(std::begin(phys_and_math), std::end(phys_and_math),
              std::ostream_iterator <Student> {std::cout, "  "});
  std::cout << std::endl;

  // List students studying physics, maths and astronomy...
  auto astronomy = courses.find("Astronomy");
  if(astronomy == std::end(courses)) throw std::invalid_argument{"Invalid course name."};
  std::cout << "\nStudents studying physics, maths, and astronomy are:\n";
  std::set_intersection(std::begin(astronomy->second), std::end(astronomy->second),
                        std::begin(phys_and_math), std::end(phys_and_math),
    std::ostream_iterator<Student>{std::cout, "  "});
  std::cout << std::endl;

  // List students studying drama or philosophy, but not both...
  auto drama = courses.find("Drama");
  auto philosophy = courses.find("Philosophy");
  if(drama == std::end(courses) || philosophy == std::end(courses)) throw std::invalid_argument{"Invalid course name."};
  Group act_or_think;
  std::cout << "\nStudents studying either drama or philosophy are:\n";
  std::set_symmetric_difference(std::begin(drama->second), std::end(drama->second),
                                std::begin(philosophy->second), std::end(philosophy->second),
                                std::inserter(act_or_think, std::begin(act_or_think)));
  std::copy(std::begin(act_or_think), std::end(act_or_think), std::ostream_iterator<Student>{std::cout, "  "});
  std::cout << std::endl;

  // Students studying drama, philosophy, or both...
  act_or_think.clear();                           // Empty the container to reuse it
  std::cout << "\nStudents studying drama and/or philosophy are:\n";
  std::set_union(std::begin(drama->second), std::end(drama->second),
    std::begin(philosophy->second), std::end(philosophy->second),
    std::inserter(act_or_think, std::begin(act_or_think)));
  std::copy(std::begin(act_or_think), std::end(act_or_think), std::ostream_iterator<Student>{std::cout, "  "});
  std::cout << std::endl;

}