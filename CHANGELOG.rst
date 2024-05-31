^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package micro_ros_utilities
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

5.0.1 (2024-05-31)
------------------
* get_longest_member_name: fix typo (`#58 <https://github.com/micro-ROS/micro_ros_utilities/issues/58>`_) (`#59 <https://github.com/micro-ROS/micro_ros_utilities/issues/59>`_)
  (cherry picked from commit 74f7a6b08690079ecbb0262db563aa96f667277d)
  Co-authored-by: G.A. vd. Hoorn <g.a.vanderhoorn@tudelft.nl>
* Remove wrong image path (`#55 <https://github.com/micro-ROS/micro_ros_utilities/issues/55>`_) (`#57 <https://github.com/micro-ROS/micro_ros_utilities/issues/57>`_)
  (cherry picked from commit 5f1cead35a1c4940db60e04e8a73c057ea2449b8)
  Co-authored-by: Antonio Cuadros <49162117+Acuadros95@users.noreply.github.com>
* Fix doxygen build (`#53 <https://github.com/micro-ROS/micro_ros_utilities/issues/53>`_) (`#54 <https://github.com/micro-ROS/micro_ros_utilities/issues/54>`_)
  (cherry picked from commit 9a3e25638db7a4629f4976ad38f2ed9d62039183)
  Co-authored-by: Antonio Cuadros <49162117+Acuadros95@users.noreply.github.com>
* Contributors: mergify[bot]

5.0.0 (2023-06-06)
------------------
* Deprecate galactic (`#52 <https://github.com/micro-ROS/micro_ros_utilities/issues/52>`_)
* Handle sequence upper bounds (`#48 <https://github.com/micro-ROS/micro_ros_utilities/issues/48>`_) (`#50 <https://github.com/micro-ROS/micro_ros_utilities/issues/50>`_)
  * Take upper bounds into account
  * Uncrusty
  (cherry picked from commit 008b94ab857c72a4e79239cb1d5b80e80ed268bc)
  Co-authored-by: Pablo Garrido <pablogs9@gmail.com>
* Fix snprintf type (`#38 <https://github.com/micro-ROS/micro_ros_utilities/issues/38>`_) (`#39 <https://github.com/micro-ROS/micro_ros_utilities/issues/39>`_)
  (cherry picked from commit 9103e7bcd298124aba94621cfc06715dcc10415b)
  Co-authored-by: Antonio Cuadros <49162117+Acuadros95@users.noreply.github.com>
* Fix nightly CI name (backport `#44 <https://github.com/micro-ROS/micro_ros_utilities/issues/44>`_) (`#45 <https://github.com/micro-ROS/micro_ros_utilities/issues/45>`_)
  * Fix nightly CI name (`#44 <https://github.com/micro-ROS/micro_ros_utilities/issues/44>`_)
  * Fix nightly name
  * Remove CI launch on push
  * Update CI versions
  (cherry picked from commit 31e017dc06e2ab4c9f0822100638cd919dbb7872)
  * Update action-ros-ci version
  Co-authored-by: Antonio Cuadros <49162117+Acuadros95@users.noreply.github.com>
  Co-authored-by: acuadros95 <acuadros1995@gmail.com>
* Modify CI approach (backport `#41 <https://github.com/micro-ROS/micro_ros_utilities/issues/41>`_) (`#42 <https://github.com/micro-ROS/micro_ros_utilities/issues/42>`_)
  * Modify CI approach (`#41 <https://github.com/micro-ROS/micro_ros_utilities/issues/41>`_)
  (cherry picked from commit fbb2248aff2e5157a5bd21a5fd072b7acfaba166)
  # Conflicts:
  #	.github/workflows/ci.yml
  * Update
  * Remove ros testing sources
  Co-authored-by: Antonio Cuadros <49162117+Acuadros95@users.noreply.github.com>
  Co-authored-by: acuadros95 <acuadros1995@gmail.com>
* Contributors: Antonio Cuadros, mergify[bot]

4.0.0 (2022-05-26)
------------------
* Update CI for Humble
* Fix struct doxygen doc (`#34 <https://github.com/micro-ROS/micro_ros_utilities/issues/34>`_) (`#35 <https://github.com/micro-ROS/micro_ros_utilities/issues/35>`_)
* Fix minor typo (`#32 <https://github.com/micro-ROS/micro_ros_utilities/issues/32>`_) (`#33 <https://github.com/micro-ROS/micro_ros_utilities/issues/33>`_)
* Document memory_conf_t and memory_rule_t members (`#30 <https://github.com/micro-ROS/micro_ros_utilities/issues/30>`_) (`#31 <https://github.com/micro-ROS/micro_ros_utilities/issues/31>`_)
* Add -Werror (`#28 <https://github.com/micro-ROS/micro_ros_utilities/issues/28>`_)
* Add -Werror (`#26 <https://github.com/micro-ROS/micro_ros_utilities/issues/26>`_)
* Fix CI (`#24 <https://github.com/micro-ROS/micro_ros_utilities/issues/24>`_)
* Fix cpplint include (`#22 <https://github.com/micro-ROS/micro_ros_utilities/issues/22>`_)
* Refactor tests (`#20 <https://github.com/micro-ROS/micro_ros_utilities/issues/20>`_) (`#21 <https://github.com/micro-ROS/micro_ros_utilities/issues/21>`_)
* Fix string sequence bug (`#18 <https://github.com/micro-ROS/micro_ros_utilities/issues/18>`_) (`#19 <https://github.com/micro-ROS/micro_ros_utilities/issues/19>`_)
* Print size_t in a portable way (`#16 <https://github.com/micro-ROS/micro_ros_utilities/issues/16>`_)
* snprintf() needs to be explicitly included (`#14 <https://github.com/micro-ROS/micro_ros_utilities/issues/14>`_) (`#15 <https://github.com/micro-ROS/micro_ros_utilities/issues/15>`_)
* Fix consts API (`#13 <https://github.com/micro-ROS/micro_ros_utilities/issues/13>`_)
* Reduce dynamic operations when using rules (`#11 <https://github.com/micro-ROS/micro_ros_utilities/issues/11>`_)
* Add regression test (`#10 <https://github.com/micro-ROS/micro_ros_utilities/issues/10>`_)
* Simplify alignment in static pools (`#9 <https://github.com/micro-ROS/micro_ros_utilities/issues/9>`_)
* Remove typedef from doxygen build (`#8 <https://github.com/micro-ROS/micro_ros_utilities/issues/8>`_)
* Fix CI (`#7 <https://github.com/micro-ROS/micro_ros_utilities/issues/7>`_)
* Add Doxygen (`#6 <https://github.com/micro-ROS/micro_ros_utilities/issues/6>`_)
* Initial release (`#4 <https://github.com/micro-ROS/micro_ros_utilities/issues/4>`_)
* Galactic CI (`#3 <https://github.com/micro-ROS/micro_ros_utilities/issues/3>`_)
* Galactic CI (`#2 <https://github.com/micro-ROS/micro_ros_utilities/issues/2>`_)
* Initial rolling (`#1 <https://github.com/micro-ROS/micro_ros_utilities/issues/1>`_)
