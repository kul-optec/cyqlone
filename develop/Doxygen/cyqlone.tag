<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.16.1" doxygen_gitid="669aeeefca743c148e2d935b3d3c69535c7491e6">
  <compound kind="file">
    <name>README.md</name>
    <path>benchmarks/cyqpalm/</path>
    <filename>benchmarks_2cyqpalm_2README_8md.html</filename>
  </compound>
  <compound kind="file">
    <name>README.md</name>
    <path></path>
    <filename>README_8md.html</filename>
  </compound>
  <compound kind="file">
    <name>algorithms.md</name>
    <path>docs/source/doxygen/</path>
    <filename>algorithms_8md.html</filename>
  </compound>
  <compound kind="file">
    <name>examples.md</name>
    <path>docs/source/doxygen/</path>
    <filename>examples_8md.html</filename>
  </compound>
  <compound kind="file">
    <name>groups.dox</name>
    <path>docs/source/doxygen/</path>
    <filename>groups_8dox.html</filename>
  </compound>
  <compound kind="file">
    <name>solve-block-tridiagonal.cpp</name>
    <path>examples/</path>
    <filename>solve-block-tridiagonal_8cpp.html</filename>
    <includes id="cyqlone_8hpp" name="cyqlone.hpp" local="no" import="no" module="no" objc="no">cyqlone/cyqlone.hpp</includes>
    <includes id="packing_8hpp" name="packing.hpp" local="no" import="no" module="no" objc="no">cyqlone/packing.hpp</includes>
    <class kind="struct">TridiagSystem</class>
    <member kind="typedef">
      <type>batmat::matrix::Matrix&lt; real_t, index_t &gt;</type>
      <name>matrices</name>
      <anchorfile>solve-block-tridiagonal_8cpp.html</anchorfile>
      <anchor>a93ebb2b3c08761f352b56f5a0ffa5606</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>cyqlone::TricyqleSolver&lt; v, real_t &gt;</type>
      <name>Solver</name>
      <anchorfile>solve-block-tridiagonal_8cpp.html</anchorfile>
      <anchor>a0c3eb48e479b222199064a957874ffee</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>TridiagSystem</type>
      <name>init_random_system</name>
      <anchorfile>solve-block-tridiagonal_8cpp.html</anchorfile>
      <anchor>aea5086e4f91c5ef217def0c6e0a7b3e6</anchor>
      <arglist>(index_t block_size, index_t num_blocks, bool circular=false)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>solve-block-tridiagonal_8cpp.html</anchorfile>
      <anchor>a0ddf1224851353fc92bfbff6f499fa97</anchor>
      <arglist>(int argc, char *argv[])</arglist>
    </member>
    <member kind="variable">
      <type>constexpr index_t</type>
      <name>v</name>
      <anchorfile>solve-block-tridiagonal_8cpp.html</anchorfile>
      <anchor>ac818a8f049bd46d6d6996d28f22de4a9</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>barrier.hpp</name>
    <path>src/cyqlone/include/cyqlone/</path>
    <filename>barrier_8hpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <class kind="struct">cyqlone::EmptyCompletion</class>
    <class kind="class">cyqlone::TreeBarrier</class>
    <class kind="class">cyqlone::TreeBarrier::arrival_token</class>
    <class kind="class">cyqlone::TreeBarrier::arrival_token_typed</class>
    <class kind="struct">cyqlone::TreeBarrier::Storage</class>
    <class kind="struct">cyqlone::TreeBarrier::State</class>
    <namespace>cyqlone</namespace>
    <member kind="define">
      <type>#define</type>
      <name>CYQLONE_SANITY_CHECKS_BARRIER</name>
      <anchorfile>barrier_8hpp.html</anchorfile>
      <anchor>a4d1e252dc13b658ff525ba615f94ae8b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>config.hpp</name>
    <path>src/cyqlone/include/cyqlone/</path>
    <filename>config_8hpp.html</filename>
    <namespace>cyqlone</namespace>
    <member kind="define">
      <type>#define</type>
      <name>CYQLONE_NS</name>
      <anchorfile>config_8hpp.html</anchorfile>
      <anchor>a1f4757b3a02c15cce8aeaa6906e55c17</anchor>
      <arglist>(ns)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>conversion.hpp</name>
    <path>src/cyqlone/include/cyqlone/</path>
    <filename>conversion_8hpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <includes id="ocp_8hpp" name="ocp.hpp" local="no" import="no" module="no" objc="no">cyqlone/ocp.hpp</includes>
    <class kind="struct">cyqlone::LinearOCPSparseQP</class>
    <class kind="struct">cyqlone::LinearOCPSparseQP::KKTMatrix</class>
    <class kind="struct">cyqlone::SparseCSC</class>
    <namespace>cyqlone</namespace>
  </compound>
  <compound kind="file">
    <name>cyqlone-params.hpp</name>
    <path>src/cyqlone/include/cyqlone/</path>
    <filename>cyqlone-params_8hpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <class kind="struct">cyqlone::TricyqleParams</class>
    <class kind="struct">cyqlone::CyqloneParams</class>
    <namespace>cyqlone</namespace>
  </compound>
  <compound kind="file">
    <name>cyqlone-storage.hpp</name>
    <path>src/cyqlone/include/cyqlone/</path>
    <filename>cyqlone-storage_8hpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <includes id="ocp_8hpp" name="ocp.hpp" local="no" import="no" module="no" objc="no">cyqlone/ocp.hpp</includes>
    <class kind="struct">cyqlone::CyqloneStorage</class>
    <namespace>cyqlone</namespace>
  </compound>
  <compound kind="file">
    <name>cyqlone.hpp</name>
    <path>src/cyqlone/include/cyqlone/</path>
    <filename>cyqlone_8hpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <includes id="cyqlone-params_8hpp" name="cyqlone-params.hpp" local="no" import="no" module="no" objc="no">cyqlone/cyqlone-params.hpp</includes>
    <includes id="cyqlone-storage_8hpp" name="cyqlone-storage.hpp" local="no" import="no" module="no" objc="no">cyqlone/cyqlone-storage.hpp</includes>
    <includes id="parallel_8hpp" name="parallel.hpp" local="no" import="no" module="no" objc="no">cyqlone/parallel.hpp</includes>
    <includes id="sparse_8hpp" name="sparse.hpp" local="no" import="no" module="no" objc="no">cyqlone/sparse.hpp</includes>
    <includes id="timing_8hpp" name="timing.hpp" local="no" import="no" module="no" objc="no">cyqlone/timing.hpp</includes>
    <class kind="struct">cyqlone::TricyqleSolver</class>
    <class kind="struct">cyqlone::CyqloneSolver</class>
    <namespace>cyqlone</namespace>
  </compound>
  <compound kind="file">
    <name>cr.tpp</name>
    <path>src/cyqlone/include/cyqlone/implementation/</path>
    <filename>cr_8tpp.html</filename>
    <includes id="cyqlone_8hpp" name="cyqlone.hpp" local="no" import="no" module="no" objc="no">cyqlone/cyqlone.hpp</includes>
    <includes id="linalg_8hpp" name="linalg.hpp" local="no" import="no" module="no" objc="no">cyqlone/linalg.hpp</includes>
    <includes id="tracing_8hpp" name="tracing.hpp" local="no" import="no" module="no" objc="no">cyqlone/tracing.hpp</includes>
    <namespace>cyqlone</namespace>
  </compound>
  <compound kind="file">
    <name>data.tpp</name>
    <path>src/cyqlone/include/cyqlone/implementation/</path>
    <filename>data_8tpp.html</filename>
    <includes id="cyqlone_8hpp" name="cyqlone.hpp" local="no" import="no" module="no" objc="no">cyqlone/cyqlone.hpp</includes>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::detail</namespace>
  </compound>
  <compound kind="file">
    <name>factor.tpp</name>
    <path>src/cyqlone/include/cyqlone/implementation/</path>
    <filename>factor_8tpp.html</filename>
    <includes id="cyqlone_8hpp" name="cyqlone.hpp" local="no" import="no" module="no" objc="no">cyqlone/cyqlone.hpp</includes>
    <namespace>cyqlone</namespace>
  </compound>
  <compound kind="file">
    <name>indexing.tpp</name>
    <path>src/cyqlone/include/cyqlone/implementation/</path>
    <filename>indexing_8tpp.html</filename>
    <includes id="cyqlone_8hpp" name="cyqlone.hpp" local="no" import="no" module="no" objc="no">cyqlone/cyqlone.hpp</includes>
    <namespace>cyqlone</namespace>
  </compound>
  <compound kind="file">
    <name>mat-vec.tpp</name>
    <path>src/cyqlone/include/cyqlone/implementation/</path>
    <filename>mat-vec_8tpp.html</filename>
    <includes id="cyqlone_8hpp" name="cyqlone.hpp" local="no" import="no" module="no" objc="no">cyqlone/cyqlone.hpp</includes>
    <includes id="linalg_8hpp" name="linalg.hpp" local="no" import="no" module="no" objc="no">cyqlone/linalg.hpp</includes>
    <namespace>cyqlone</namespace>
  </compound>
  <compound kind="file">
    <name>pcg.tpp</name>
    <path>src/cyqlone/include/cyqlone/implementation/</path>
    <filename>pcg_8tpp.html</filename>
    <includes id="cyqlone_8hpp" name="cyqlone.hpp" local="no" import="no" module="no" objc="no">cyqlone/cyqlone.hpp</includes>
    <includes id="linalg_8hpp" name="linalg.hpp" local="no" import="no" module="no" objc="no">cyqlone/linalg.hpp</includes>
    <namespace>cyqlone</namespace>
  </compound>
  <compound kind="file">
    <name>pcr.tpp</name>
    <path>src/cyqlone/include/cyqlone/implementation/</path>
    <filename>pcr_8tpp.html</filename>
    <includes id="cyqlone_8hpp" name="cyqlone.hpp" local="no" import="no" module="no" objc="no">cyqlone/cyqlone.hpp</includes>
    <includes id="linalg_8hpp" name="linalg.hpp" local="no" import="no" module="no" objc="no">cyqlone/linalg.hpp</includes>
    <namespace>cyqlone</namespace>
  </compound>
  <compound kind="file">
    <name>riccati.tpp</name>
    <path>src/cyqlone/include/cyqlone/implementation/</path>
    <filename>riccati_8tpp.html</filename>
    <includes id="cyqlone_8hpp" name="cyqlone.hpp" local="no" import="no" module="no" objc="no">cyqlone/cyqlone.hpp</includes>
    <includes id="linalg_8hpp" name="linalg.hpp" local="no" import="no" module="no" objc="no">cyqlone/linalg.hpp</includes>
    <namespace>cyqlone</namespace>
  </compound>
  <compound kind="file">
    <name>schur.tpp</name>
    <path>src/cyqlone/include/cyqlone/implementation/</path>
    <filename>schur_8tpp.html</filename>
    <includes id="cyqlone_8hpp" name="cyqlone.hpp" local="no" import="no" module="no" objc="no">cyqlone/cyqlone.hpp</includes>
    <includes id="linalg_8hpp" name="linalg.hpp" local="no" import="no" module="no" objc="no">cyqlone/linalg.hpp</includes>
    <includes id="tracing_8hpp" name="tracing.hpp" local="no" import="no" module="no" objc="no">cyqlone/tracing.hpp</includes>
    <namespace>cyqlone</namespace>
  </compound>
  <compound kind="file">
    <name>sparse.tpp</name>
    <path>src/cyqlone/include/cyqlone/implementation/</path>
    <filename>sparse_8tpp.html</filename>
    <includes id="cyqlone_8hpp" name="cyqlone.hpp" local="no" import="no" module="no" objc="no">cyqlone/cyqlone.hpp</includes>
    <namespace>cyqlone</namespace>
  </compound>
  <compound kind="file">
    <name>update.tpp</name>
    <path>src/cyqlone/include/cyqlone/implementation/</path>
    <filename>update_8tpp.html</filename>
    <includes id="cyqlone_8hpp" name="cyqlone.hpp" local="no" import="no" module="no" objc="no">cyqlone/cyqlone.hpp</includes>
    <includes id="tracing_8hpp" name="tracing.hpp" local="no" import="no" module="no" objc="no">cyqlone/tracing.hpp</includes>
    <namespace>cyqlone</namespace>
  </compound>
  <compound kind="file">
    <name>linalg.hpp</name>
    <path>src/cyqlone/include/cyqlone/</path>
    <filename>linalg_8hpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <includes id="reduce_8hpp" name="reduce.hpp" local="no" import="no" module="no" objc="no">cyqlone/reduce.hpp</includes>
    <member kind="function">
      <type>norms&lt; simdified_value_t&lt; Vx &gt; &gt;::result</type>
      <name>norms_all</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga5b021d3e61d99e49576a574e7c919923</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>norm_inf</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga2332e39f282f0b22d60d5e0a57f4e932</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>norm_1</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga972ef7eb9a7e6a2c3680dfb95737e083</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>norm_2_squared</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga60918683b4f31e9f30b1b40fb12fdc97</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>norm_2</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gae11e8e17f4b74608291fe5b2d389869c</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>dot</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gac25c43a1c180bb6ad0faedbde44a20a9</anchor>
      <arglist>(Vx &amp;&amp;x, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scale</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaeee444a2231976efc5f93a58cada45c5</anchor>
      <arglist>(T alpha, Vx &amp;&amp;x, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scale</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga16a11c4519ab1e0cf4946c57bf4d0671</anchor>
      <arglist>(T alpha, Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>hadamard</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gae8ee607462b95f6106dc714e1fe86f60</anchor>
      <arglist>(Vx &amp;&amp;x, Vy &amp;&amp;y, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>hadamard</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga5f7dead7502413b423294c480fe67df7</anchor>
      <arglist>(Vx &amp;&amp;x, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clamp</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gacd0f6a1148300c8895b047baafd80619</anchor>
      <arglist>(Vx &amp;&amp;x, Vlo &amp;&amp;lo, Vhi &amp;&amp;hi, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clamp_resid</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga7c05b45f1fc5c69ed29a8cb7f0d4cd33</anchor>
      <arglist>(Vx &amp;&amp;x, Vlo &amp;&amp;lo, Vhi &amp;&amp;hi, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>axpby</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga7798d7d6e1ab0065fd4abc60fcc2e437</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Tb beta, Vy &amp;&amp;y, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>axpby</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga7be0c0501654d83ed8cc615b5d19d4e9</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Tb beta, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>axpy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga3f4f3b9f53f734e9a29368d36a5d80a0</anchor>
      <arglist>(Vy &amp;&amp;y, const std::array&lt; simdified_value_t&lt; Vy &gt;, sizeof...(Vx)&gt; &amp;alphas, Vx &amp;&amp;...x)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>axpy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaf92a45bb3dfc15de8dcd96ee9bd2345b</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Vy &amp;&amp;y, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>axpy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga6a42b0d6640680263a1c97afd7a6e2cb</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>negate</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga8107de760e8af9ee6dddc4f9878c1339</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, with_rotate_t&lt; Rotate &gt;={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>negate</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaae5b326a925859f5c11cddbd75b1b057</anchor>
      <arglist>(VA &amp;&amp;A, with_rotate_t&lt; Rotate &gt;={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sub</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gafb26638c219b8754db25e88f69304439</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, VC &amp;&amp;C, with_rotate_t&lt; Rotate &gt;={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sub</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga907ac42d74d99fe2a6abc9c4fcd80b2b</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, with_rotate_t&lt; Rotate &gt;={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga63e5d7edfbf2831545aea00d9a75966b</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, VC &amp;&amp;C, with_rotate_t&lt; Rotate &gt;={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gab6af7e9086aefaf875651e751e1ad92b</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, with_rotate_t&lt; Rotate &gt;={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>for_each_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga42110f9f447dd30af3fdc936a07d1a39</anchor>
      <arglist>(F &amp;&amp;fun, VA &amp;&amp;A, VAs &amp;&amp;...As)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>transform_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga5b35f1651072845d48695ccefa5ef549</anchor>
      <arglist>(F &amp;&amp;fun, VA &amp;&amp;A, VAs &amp;&amp;...As)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>transform2_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gacaef92140287b1c3e3c41c2a71ee69ab</anchor>
      <arglist>(F &amp;&amp;fun, VA &amp;&amp;A, VB &amp;&amp;B, VAs &amp;&amp;...As)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>transform_n_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaaa852f170922addceaa82a4cb63349c6</anchor>
      <arglist>(F &amp;&amp;fun, std::tuple&lt; VAs... &gt; As, VBs &amp;&amp;...Bs)</arglist>
    </member>
    <member kind="function">
      <type>norms&lt; simdified_value_t&lt; Vx &gt; &gt;::result</type>
      <name>norms_all</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gade4eb9023d1b94d7657f68dc256860b8</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>norm_inf</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga55c086fc7235a79e26c3aa5b1154c224</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>norm_1</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gabf875f22442b0023c2142b00d0975554</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>norm_2_squared</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gae934751791c316edde36013271945bf3</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>norm_2</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga91b1ca93bfba644b53749eeac8edb444</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>dot</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gafac39321d4eca4894c8099e597a9b5f2</anchor>
      <arglist>(Vx &amp;&amp;x, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scale</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gad40329bb0eea1a12dd5b510935b93fca</anchor>
      <arglist>(T alpha, Vx &amp;&amp;x, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scale</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga4dd6dc0ecf27af5d2f74f12e0aaca26a</anchor>
      <arglist>(T alpha, Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>hadamard</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga192f6d5da228f92a9e16216064fdac0e</anchor>
      <arglist>(Vx &amp;&amp;x, Vy &amp;&amp;y, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>hadamard</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga970509394d0f01a224acb3eac0f97535</anchor>
      <arglist>(Vx &amp;&amp;x, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clamp</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaaea202adb9d942b00a786f902e45307d</anchor>
      <arglist>(Vx &amp;&amp;x, Vlo &amp;&amp;lo, Vhi &amp;&amp;hi, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clamp_resid</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga60b2783e234cd9c038d1067b4f529a38</anchor>
      <arglist>(Vx &amp;&amp;x, Vlo &amp;&amp;lo, Vhi &amp;&amp;hi, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>axpby</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga2c0d5fb8f9ea81725e5d54d7605129e3</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Tb beta, Vy &amp;&amp;y, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>axpby</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaca21e3c51e47359f16410a524f1a6896</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Tb beta, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>axpy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaede77b28f5e46753ee7b915935f144b5</anchor>
      <arglist>(Vy &amp;&amp;y, const std::array&lt; simdified_value_t&lt; Vy &gt;, sizeof...(Vx)&gt; &amp;alphas, Vx &amp;&amp;...x)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>axpy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga30451a0e4f4100069ff65963d1bb0dbf</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Vy &amp;&amp;y, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>axpy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga74efe6fd79de11b4fad66ce0444c2de3</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>negate</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga4024401b27cb17a5982d63dca91df61b</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, with_rotate_t&lt; Rotate &gt; rot={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>negate</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gadfa94d9946d30ead1c8a0896599bd313</anchor>
      <arglist>(VA &amp;&amp;A, with_rotate_t&lt; Rotate &gt; rot={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sub</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaff7f734df0a2b041994f963e2226f858</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, VC &amp;&amp;C, with_rotate_t&lt; Rotate &gt; rot={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sub</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga48d07be903d60f6d48b7c3af0e9da414</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, with_rotate_t&lt; Rotate &gt; rot={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gac534f2ffcc1821dedd98afd67b4d5e27</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, VC &amp;&amp;C, with_rotate_t&lt; Rotate &gt; rot={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga84c12f9e93355fcc36107e18086ef7a4</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, with_rotate_t&lt; Rotate &gt; rot={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>for_each_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaa72458de29526c1deb6a0519926b2cd0</anchor>
      <arglist>(F &amp;&amp;fun, VA &amp;&amp;A, VAs &amp;&amp;...As)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>transform_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga04fa9fa6e82563be75524b11f135bc2b</anchor>
      <arglist>(F &amp;&amp;fun, VA &amp;&amp;A, VAs &amp;&amp;...As)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>transform2_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga79cf0bb8d436a1cba7ce70fab44bfbf0</anchor>
      <arglist>(F &amp;&amp;fun, VA &amp;&amp;A, VB &amp;&amp;B, VAs &amp;&amp;...As)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>transform_n_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga52c5beff69e8bdfb213cd47caacc104f</anchor>
      <arglist>(F &amp;&amp;fun, std::tuple&lt; VAs... &gt; As, VBs &amp;&amp;...Bs)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>copy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga85860ca1111492777572a2ba091cf5ca</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, Opts... opts)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>copy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga2e50e5f96654eb673d29ac8461143e91</anchor>
      <arglist>(Structured&lt; VA, S &gt; A, Structured&lt; VB, S &gt; B, Opts... opts)</arglist>
    </member>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::linalg</namespace>
    <namespace>cyqlone::linalg::multi</namespace>
  </compound>
  <compound kind="file">
    <name>matio.hpp</name>
    <path>src/cyqlone/include/cyqlone/</path>
    <filename>matio_8hpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <includes id="ocp_8hpp" name="ocp.hpp" local="no" import="no" module="no" objc="no">cyqlone/ocp.hpp</includes>
    <includes id="sparse_8hpp" name="sparse.hpp" local="no" import="no" module="no" objc="no">cyqlone/sparse.hpp</includes>
    <namespace>cyqlone</namespace>
  </compound>
  <compound kind="file">
    <name>neumaier.hpp</name>
    <path>src/cyqlone/include/cyqlone/</path>
    <filename>neumaier_8hpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <class kind="class">cyqlone::NeumaierSum</class>
    <namespace>cyqlone</namespace>
  </compound>
  <compound kind="file">
    <name>ocp.hpp</name>
    <path>src/cyqlone/include/cyqlone/</path>
    <filename>ocp_8hpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <class kind="struct">cyqlone::OCPDim</class>
    <class kind="struct">cyqlone::LinearOCPStorage</class>
    <class kind="struct">cyqlone::LinearOCPStorage::Solution</class>
    <class kind="struct">cyqlone::LinearOCPStorage::KKTError</class>
    <namespace>cyqlone</namespace>
  </compound>
  <compound kind="file">
    <name>packing.hpp</name>
    <path>src/cyqlone/include/cyqlone/</path>
    <filename>packing_8hpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <member kind="function">
      <type>void</type>
      <name>unpack</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga526900bf6322e7c42895c80c36ac2f5e</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>pack</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gae29d94393be39ebcd6ce5f909e28787c</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B)</arglist>
    </member>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::linalg</namespace>
  </compound>
  <compound kind="file">
    <name>parallel.hpp</name>
    <path>src/cyqlone/include/cyqlone/</path>
    <filename>parallel_8hpp.html</filename>
    <includes id="barrier_8hpp" name="barrier.hpp" local="no" import="no" module="no" objc="no">cyqlone/barrier.hpp</includes>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <class kind="struct">cyqlone::parallel::SharedContext</class>
    <class kind="struct">cyqlone::parallel::Context</class>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::parallel</namespace>
  </compound>
  <compound kind="file">
    <name>random-ocp.hpp</name>
    <path>src/cyqlone/include/cyqlone/</path>
    <filename>random-ocp_8hpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <includes id="ocp_8hpp" name="ocp.hpp" local="no" import="no" module="no" objc="no">cyqlone/ocp.hpp</includes>
    <namespace>cyqlone</namespace>
  </compound>
  <compound kind="file">
    <name>reduce.hpp</name>
    <path>src/cyqlone/include/cyqlone/</path>
    <filename>reduce_8hpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <class kind="struct">cyqlone::norms</class>
    <class kind="struct">cyqlone::norms::result_simd</class>
    <class kind="struct">cyqlone::norms&lt; T, void &gt;</class>
    <class kind="struct">cyqlone::norms&lt; T, void &gt;::result</class>
    <namespace>cyqlone</namespace>
  </compound>
  <compound kind="file">
    <name>sparse.hpp</name>
    <path>src/cyqlone/include/cyqlone/</path>
    <filename>sparse_8hpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <class kind="struct">cyqlone::SparseMatrix</class>
    <class kind="struct">cyqlone::SparseMatrixBuilder</class>
    <class kind="struct">cyqlone::SparseCOO</class>
    <namespace>cyqlone</namespace>
  </compound>
  <compound kind="file">
    <name>timing.hpp</name>
    <path>src/cyqlone/include/cyqlone/</path>
    <filename>timing_8hpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <namespace>cyqlone</namespace>
  </compound>
  <compound kind="file">
    <name>tracing.hpp</name>
    <path>src/cyqlone/include/cyqlone/</path>
    <filename>tracing_8hpp.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>CYQ_TRACE_WRITE</name>
      <anchorfile>tracing_8hpp.html</anchorfile>
      <anchor>aa73a42b98ae9dc83b1ba49bbaa8b9f78</anchor>
      <arglist>(...)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>CYQ_TRACE_READ</name>
      <anchorfile>tracing_8hpp.html</anchorfile>
      <anchor>af06e8f0353c7efc7707c5d97747c4673</anchor>
      <arglist>(...)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>conversion.cpp</name>
    <path>src/cyqlone/src/</path>
    <filename>conversion_8cpp.html</filename>
    <includes id="conversion_8hpp" name="conversion.hpp" local="no" import="no" module="no" objc="no">cyqlone/conversion.hpp</includes>
    <namespace>cyqlone</namespace>
  </compound>
  <compound kind="file">
    <name>cyqlone-storage.cpp</name>
    <path>src/cyqlone/src/</path>
    <filename>cyqlone-storage_8cpp.html</filename>
    <includes id="cyqlone-storage_8hpp" name="cyqlone-storage.hpp" local="no" import="no" module="no" objc="no">cyqlone/cyqlone-storage.hpp</includes>
    <namespace>cyqlone</namespace>
  </compound>
  <compound kind="file">
    <name>cyqlone.cpp</name>
    <path>src/cyqlone/src/</path>
    <filename>cyqlone_8cpp.html</filename>
    <includes id="cyqlone_8hpp" name="cyqlone.hpp" local="no" import="no" module="no" objc="no">cyqlone/cyqlone.hpp</includes>
    <includes id="cr_8tpp" name="cr.tpp" local="no" import="no" module="no" objc="no">cyqlone/implementation/cr.tpp</includes>
    <includes id="data_8tpp" name="data.tpp" local="no" import="no" module="no" objc="no">cyqlone/implementation/data.tpp</includes>
    <includes id="factor_8tpp" name="factor.tpp" local="no" import="no" module="no" objc="no">cyqlone/implementation/factor.tpp</includes>
    <includes id="indexing_8tpp" name="indexing.tpp" local="no" import="no" module="no" objc="no">cyqlone/implementation/indexing.tpp</includes>
    <includes id="mat-vec_8tpp" name="mat-vec.tpp" local="no" import="no" module="no" objc="no">cyqlone/implementation/mat-vec.tpp</includes>
    <includes id="pcg_8tpp" name="pcg.tpp" local="no" import="no" module="no" objc="no">cyqlone/implementation/pcg.tpp</includes>
    <includes id="pcr_8tpp" name="pcr.tpp" local="no" import="no" module="no" objc="no">cyqlone/implementation/pcr.tpp</includes>
    <includes id="riccati_8tpp" name="riccati.tpp" local="no" import="no" module="no" objc="no">cyqlone/implementation/riccati.tpp</includes>
    <includes id="schur_8tpp" name="schur.tpp" local="no" import="no" module="no" objc="no">cyqlone/implementation/schur.tpp</includes>
    <includes id="sparse_8tpp" name="sparse.tpp" local="no" import="no" module="no" objc="no">cyqlone/implementation/sparse.tpp</includes>
    <includes id="update_8tpp" name="update.tpp" local="no" import="no" module="no" objc="no">cyqlone/implementation/update.tpp</includes>
    <namespace>cyqlone</namespace>
    <member kind="define">
      <type>#define</type>
      <name>CYQLONE_INSTANTIATE_TRICYQLE</name>
      <anchorfile>cyqlone_8cpp.html</anchorfile>
      <anchor>a39e072dda5243754590685f16f67e848</anchor>
      <arglist>(T, VL, O)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>CYQLONE_INSTANTIATE_CYQLONE</name>
      <anchorfile>cyqlone_8cpp.html</anchorfile>
      <anchor>a56700da47cd2ddd2e55e71f57e5aedb4</anchor>
      <arglist>(VL, O)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>matio.cpp</name>
    <path>src/cyqlone/src/</path>
    <filename>matio_8cpp.html</filename>
    <includes id="matio_8hpp" name="matio.hpp" local="no" import="no" module="no" objc="no">cyqlone/matio.hpp</includes>
    <includes id="ocp_8hpp" name="ocp.hpp" local="no" import="no" module="no" objc="no">cyqlone/ocp.hpp</includes>
    <class kind="struct">cyqlone::matio_traits&lt; float &gt;</class>
    <class kind="struct">cyqlone::matio_traits&lt; double &gt;</class>
    <class kind="struct">cyqlone::matio_traits&lt; I &gt;</class>
    <namespace>cyqlone</namespace>
  </compound>
  <compound kind="file">
    <name>ocp.cpp</name>
    <path>src/cyqlone/src/</path>
    <filename>ocp_8cpp.html</filename>
    <includes id="ocp_8hpp" name="ocp.hpp" local="no" import="no" module="no" objc="no">cyqlone/ocp.hpp</includes>
    <includes id="reduce_8hpp" name="reduce.hpp" local="no" import="no" module="no" objc="no">cyqlone/reduce.hpp</includes>
    <namespace>cyqlone</namespace>
  </compound>
  <compound kind="file">
    <name>tracing.cpp</name>
    <path>src/cyqlone/src/</path>
    <filename>tracing_8cpp.html</filename>
    <includes id="tracing_8hpp" name="tracing.hpp" local="no" import="no" module="no" objc="no">cyqlone/tracing.hpp</includes>
  </compound>
  <compound kind="file">
    <name>csv.hpp</name>
    <path>src/example-problems/include/cyqlone/qpalm/example-problems/</path>
    <filename>csv_8hpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <includes id="ocp_8hpp" name="ocp.hpp" local="no" import="no" module="no" objc="no">cyqlone/ocp.hpp</includes>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::qpalm</namespace>
    <namespace>cyqlone::qpalm::problems</namespace>
  </compound>
  <compound kind="file">
    <name>platooning.hpp</name>
    <path>src/example-problems/include/cyqlone/qpalm/example-problems/</path>
    <filename>platooning_8hpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <includes id="ocp_8hpp" name="ocp.hpp" local="no" import="no" module="no" objc="no">cyqlone/ocp.hpp</includes>
    <class kind="struct">cyqlone::qpalm::problems::PlatooningParams</class>
    <class kind="struct">cyqlone::qpalm::problems::PlatooningProblem</class>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::qpalm</namespace>
    <namespace>cyqlone::qpalm::problems</namespace>
  </compound>
  <compound kind="file">
    <name>spring-mass.hpp</name>
    <path>src/example-problems/include/cyqlone/qpalm/example-problems/</path>
    <filename>spring-mass_8hpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <includes id="ocp_8hpp" name="ocp.hpp" local="no" import="no" module="no" objc="no">cyqlone/ocp.hpp</includes>
    <class kind="struct">cyqlone::qpalm::problems::SpringMassParams</class>
    <class kind="struct">cyqlone::qpalm::problems::SpringMassProblem</class>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::qpalm</namespace>
    <namespace>cyqlone::qpalm::problems</namespace>
  </compound>
  <compound kind="file">
    <name>zoh.hpp</name>
    <path>src/example-problems/include/cyqlone/qpalm/example-problems/</path>
    <filename>zoh_8hpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::qpalm</namespace>
    <namespace>cyqlone::qpalm::problems</namespace>
  </compound>
  <compound kind="file">
    <name>csv.cpp</name>
    <path>src/example-problems/src/</path>
    <filename>csv_8cpp.html</filename>
    <includes id="csv_8hpp" name="csv.hpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/example-problems/csv.hpp</includes>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::qpalm</namespace>
    <namespace>cyqlone::qpalm::problems</namespace>
  </compound>
  <compound kind="file">
    <name>platooning.cpp</name>
    <path>src/example-problems/src/</path>
    <filename>platooning_8cpp.html</filename>
    <includes id="conversion_8hpp" name="conversion.hpp" local="no" import="no" module="no" objc="no">cyqlone/conversion.hpp</includes>
    <includes id="platooning_8hpp" name="platooning.hpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/example-problems/platooning.hpp</includes>
    <includes id="zoh_8hpp" name="zoh.hpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/example-problems/zoh.hpp</includes>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::qpalm</namespace>
    <namespace>cyqlone::qpalm::problems</namespace>
  </compound>
  <compound kind="file">
    <name>spring-mass.cpp</name>
    <path>benchmarks/cyqpalm/</path>
    <filename>benchmarks_2cyqpalm_2spring-mass_8cpp.html</filename>
    <includes id="matio_8hpp" name="matio.hpp" local="no" import="no" module="no" objc="no">cyqlone/matio.hpp</includes>
    <includes id="ocp-backend-cyqlone_8hpp" name="ocp-backend-cyqlone.hpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/backends/ocp-backend-cyqlone.hpp</includes>
    <includes id="spring-mass_8hpp" name="spring-mass.hpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/example-problems/spring-mass.hpp</includes>
    <includes id="settings_8hpp" name="settings.hpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/settings.hpp</includes>
    <includes id="solver_8hpp" name="solver.hpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/solver.hpp</includes>
    <includes id="status_8hpp" name="status.hpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/status.hpp</includes>
    <includes id="tracing_8hpp" name="tracing.hpp" local="no" import="no" module="no" objc="no">cyqlone/tracing.hpp</includes>
    <class kind="struct">Options</class>
    <class kind="struct">Problem</class>
    <class kind="struct">Solver</class>
    <class kind="struct">SpringMassParams</class>
    <member kind="typedef">
      <type>std::chrono::duration&lt; double &gt;</type>
      <name>seconds</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>aedfb63997e73d0dc8964ae5c1f36cd41</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>ProblemType</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a4c669cb1cb4d98dfea944e9ceec7d33e</anchor>
      <arglist></arglist>
      <enumvalue file="benchmarks_2cyqpalm_2spring-mass_8cpp.html" anchor="a4c669cb1cb4d98dfea944e9ceec7d33eafc57686bf20fc8e206157d78a9fb758b">WangBoyd2008</enumvalue>
      <enumvalue file="benchmarks_2cyqpalm_2spring-mass_8cpp.html" anchor="a4c669cb1cb4d98dfea944e9ceec7d33ea25d408c56cbbbab45f40c893418fb894">WangBoyd2008Width</enumvalue>
      <enumvalue file="benchmarks_2cyqpalm_2spring-mass_8cpp.html" anchor="a4c669cb1cb4d98dfea944e9ceec7d33eae7ce5734fbb933180d31b231c2eb7be1">Domahidi2012</enumvalue>
      <enumvalue file="benchmarks_2cyqpalm_2spring-mass_8cpp.html" anchor="a4c669cb1cb4d98dfea944e9ceec7d33eae327976f658280963c254af6d143c777">ActiveStateConstr</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>WarmStartHPIPM</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a1e5ece077879bedb19586a2c036f2c23</anchor>
      <arglist></arglist>
      <enumvalue file="benchmarks_2cyqpalm_2spring-mass_8cpp.html" anchor="a1e5ece077879bedb19586a2c036f2c23a5b85379fb6d4ff2312ca180d829fdf01">NoWarmStart</enumvalue>
      <enumvalue file="benchmarks_2cyqpalm_2spring-mass_8cpp.html" anchor="a1e5ece077879bedb19586a2c036f2c23a54db44089bbacc4895e3414518231c77">WarmZero</enumvalue>
      <enumvalue file="benchmarks_2cyqpalm_2spring-mass_8cpp.html" anchor="a1e5ece077879bedb19586a2c036f2c23abe144a42394a837bd7f50a6b10cf78be">WarmCopy</enumvalue>
      <enumvalue file="benchmarks_2cyqpalm_2spring-mass_8cpp.html" anchor="a1e5ece077879bedb19586a2c036f2c23a971d78d68790b314dad6cbe5b5431d8f">WarmShift</enumvalue>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>counter</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>aed9742e190fcdab12ce85ae28d0c0a66</anchor>
      <arglist>(auto x)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>counter_avg</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a0c126e7f6f691e9434fc9c999ed9b106</anchor>
      <arglist>(auto x)</arglist>
    </member>
    <member kind="function">
      <type>qp::problems::SpringMassProblem</type>
      <name>create_problem</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a87c3add091c930e9266aaf6c71d5b499</anchor>
      <arglist>(const SpringMassParams &amp;params)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>disable_tracing</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a796f7650fc942bd964d67e29a30785c4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>trace_run</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>ad3b3d4b3fd7cf72bcb309c69323c1c1d</anchor>
      <arglist>(auto &amp;&amp;, const auto &amp;)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print_traces</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a99ac5e937e8cd6649369830168443ed2</anchor>
      <arglist>(std::ostream &amp;)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>run_benchmark</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a9a50c58845ad3929bc28241ad1ef0991</anchor>
      <arglist>(benchmark::State &amp;state, const std::string &amp;param_name, const SpringMassParams &amp;params, qp::CyQPALMBackendSettings backend_settings, qp::Settings settings, bool warm=false, bool trace=false)</arglist>
    </member>
    <member kind="function">
      <type>std::generator&lt; Problem &gt;</type>
      <name>get_spring_mass_params</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>ab1a4727f1d17d76d3fb9c497efb1650b</anchor>
      <arglist>(const Options &amp;opts)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>export_problem</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a14f468c71f063f09688477ea3fd75bb8</anchor>
      <arglist>(const Options &amp;opts)</arglist>
    </member>
    <member kind="function">
      <type>std::string_view</type>
      <name>order</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>af61fff0ad381f171d5c3d5fdbb5835d5</anchor>
      <arglist>(qp::StorageOrder o)</arglist>
    </member>
    <member kind="function">
      <type>std::generator&lt; Solver &gt;</type>
      <name>get_cyqlone_solvers</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>ac3104388361a980a90810067441fdad2</anchor>
      <arglist>(const Options &amp;opts)</arglist>
    </member>
    <member kind="function">
      <type>std::generator&lt; Solver &gt;</type>
      <name>get_hpipm_solvers</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a114b0239a8ec074b2ceec7996775c9a0</anchor>
      <arglist>(const Options &amp;opts)</arglist>
    </member>
    <member kind="function">
      <type>std::generator&lt; Solver &gt;</type>
      <name>get_cyqlone_solvers_vl</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a151b1c60490223468d41623ebfdde774</anchor>
      <arglist>(const Options &amp;opts)</arglist>
    </member>
    <member kind="function">
      <type>std::generator&lt; Solver &gt;</type>
      <name>get_solvers</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a24b505c3306cc2abef80b9bf1bab9339</anchor>
      <arglist>(const Options &amp;opts)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>register_benchmarks</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a5e87583157c03d1f72c32e615762ca84</anchor>
      <arglist>(const Options &amp;opts)</arglist>
    </member>
    <member kind="function">
      <type>std::unique_ptr&lt; benchmark::BenchmarkReporter &gt;</type>
      <name>make_custom_reporter</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a4670f9f2112847d3390329c356aaabfe</anchor>
      <arglist>(size_t problem_name_width, size_t solver_name_width, bool print_extra, bool with_color)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>register_options</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a6301de13c9af8b12550574d0b6336157</anchor>
      <arglist>(const char *program, CLI::App &amp;app, Options &amp;opts)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>initialize_google_benchmark</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a2fab40ef532103fd442a6d633bde2edc</anchor>
      <arglist>(char *program, auto bm_args)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>register_context</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a2eb0bd8e125a8ca4d991c8146f1d9f53</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a3c04138a5bfe5d72780bb7e82a18e627</anchor>
      <arglist>(int argc, char **argv)</arglist>
    </member>
    <member kind="variable">
      <type>constexpr auto</type>
      <name>v_native</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a7a4d39b575e51782c5391448b0a1f931</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>constexpr auto</type>
      <name>v</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>abd7c170326903ac2f678a7b7f48a1dca</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::map&lt; std::string, ProblemType &gt;</type>
      <name>problem_type_map</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>af601e9d42ebf311a42fa20225d866186</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>spring-mass.cpp</name>
    <path>src/example-problems/src/</path>
    <filename>src_2example-problems_2src_2spring-mass_8cpp.html</filename>
    <includes id="conversion_8hpp" name="conversion.hpp" local="no" import="no" module="no" objc="no">cyqlone/conversion.hpp</includes>
    <includes id="spring-mass_8hpp" name="spring-mass.hpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/example-problems/spring-mass.hpp</includes>
    <includes id="zoh_8hpp" name="zoh.hpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/example-problems/zoh.hpp</includes>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::qpalm</namespace>
    <namespace>cyqlone::qpalm::problems</namespace>
  </compound>
  <compound kind="file">
    <name>ineq-constr.tpp</name>
    <path>src/qpalm/include/cyqlone/qpalm/backends/backend-cyqlone/</path>
    <filename>ineq-constr_8tpp.html</filename>
    <includes id="linalg_8hpp" name="linalg.hpp" local="no" import="no" module="no" objc="no">cyqlone/linalg.hpp</includes>
    <includes id="ocp-backend-cyqlone_8tpp" name="ocp-backend-cyqlone.tpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/backends/ocp-backend-cyqlone.tpp</includes>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::qpalm</namespace>
  </compound>
  <compound kind="file">
    <name>linalg.tpp</name>
    <path>src/qpalm/include/cyqlone/qpalm/backends/backend-cyqlone/</path>
    <filename>linalg_8tpp.html</filename>
    <includes id="linalg_8hpp" name="linalg.hpp" local="no" import="no" module="no" objc="no">cyqlone/linalg.hpp</includes>
    <includes id="ocp-backend-cyqlone_8tpp" name="ocp-backend-cyqlone.tpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/backends/ocp-backend-cyqlone.tpp</includes>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::qpalm</namespace>
  </compound>
  <compound kind="file">
    <name>solve.tpp</name>
    <path>src/qpalm/include/cyqlone/qpalm/backends/backend-cyqlone/</path>
    <filename>solve_8tpp.html</filename>
    <includes id="linalg_8hpp" name="linalg.hpp" local="no" import="no" module="no" objc="no">cyqlone/linalg.hpp</includes>
    <includes id="ocp-backend-cyqlone_8tpp" name="ocp-backend-cyqlone.tpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/backends/ocp-backend-cyqlone.tpp</includes>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::qpalm</namespace>
  </compound>
  <compound kind="file">
    <name>ocp-backend-cyqlone.hpp</name>
    <path>src/qpalm/include/cyqlone/qpalm/backends/</path>
    <filename>ocp-backend-cyqlone_8hpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <includes id="cyqlone-params_8hpp" name="cyqlone-params.hpp" local="no" import="no" module="no" objc="no">cyqlone/cyqlone-params.hpp</includes>
    <includes id="cyqlone-storage_8hpp" name="cyqlone-storage.hpp" local="no" import="no" module="no" objc="no">cyqlone/cyqlone-storage.hpp</includes>
    <includes id="solver_8hpp" name="solver.hpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/solver.hpp</includes>
    <class kind="struct">cyqlone::qpalm::CyqloneData</class>
    <class kind="struct">cyqlone::qpalm::CyQPALMBackendSettings</class>
    <class kind="struct">cyqlone::qpalm::CyQPALMBackendStats</class>
    <class kind="struct">cyqlone::qpalm::detail::backend_stats_type&lt; CyQPALMBackend&lt; VL, DefaultOrder &gt; &gt;</class>
    <class kind="struct">cyqlone::qpalm::unique_CyQPALMBackend</class>
    <class kind="struct">cyqlone::qpalm::detail::backend_type&lt; unique_CyQPALMBackend&lt; VL, DefaultOrder &gt; &gt;</class>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::qpalm</namespace>
    <namespace>cyqlone::qpalm::detail</namespace>
  </compound>
  <compound kind="file">
    <name>ocp-backend-cyqlone.tpp</name>
    <path>src/qpalm/include/cyqlone/qpalm/backends/</path>
    <filename>ocp-backend-cyqlone_8tpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <includes id="cyqlone_8hpp" name="cyqlone.hpp" local="no" import="no" module="no" objc="no">cyqlone/cyqlone.hpp</includes>
    <includes id="linalg_8hpp" name="linalg.hpp" local="no" import="no" module="no" objc="no">cyqlone/linalg.hpp</includes>
    <includes id="neumaier_8hpp" name="neumaier.hpp" local="no" import="no" module="no" objc="no">cyqlone/neumaier.hpp</includes>
    <includes id="ocp-backend-cyqlone_8hpp" name="ocp-backend-cyqlone.hpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/backends/ocp-backend-cyqlone.hpp</includes>
    <includes id="breakpoint_8hpp" name="breakpoint.hpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/implementation/breakpoint.hpp</includes>
    <includes id="reduce_8hpp" name="reduce.hpp" local="no" import="no" module="no" objc="no">cyqlone/reduce.hpp</includes>
    <includes id="ineq-constr_8tpp" name="ineq-constr.tpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/backends/backend-cyqlone/ineq-constr.tpp</includes>
    <includes id="linalg_8tpp" name="linalg.tpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/backends/backend-cyqlone/linalg.tpp</includes>
    <includes id="backends_2backend-cyqlone_2linesearch_8tpp" name="linesearch.tpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/backends/backend-cyqlone/linesearch.tpp</includes>
    <includes id="solve_8tpp" name="solve.tpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/backends/backend-cyqlone/solve.tpp</includes>
    <class kind="struct">cyqlone::qpalm::CyQPALMBackend</class>
    <class kind="struct">cyqlone::qpalm::CyQPALMBackend::var_vec_t</class>
    <class kind="struct">cyqlone::qpalm::CyQPALMBackend::eq_constr_vec_t</class>
    <class kind="struct">cyqlone::qpalm::CyQPALMBackend::ineq_constr_vec_t</class>
    <class kind="struct">cyqlone::qpalm::CyQPALMBackend::active_set_t</class>
    <class kind="struct">cyqlone::qpalm::CyQPALMBackend::Timings</class>
    <class kind="struct">cyqlone::qpalm::CyQPALMBackend::PenaltySettings</class>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::qpalm</namespace>
  </compound>
  <compound kind="file">
    <name>detailed-stats.hpp</name>
    <path>src/qpalm/include/cyqlone/qpalm/</path>
    <filename>detailed-stats_8hpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <class kind="struct">cyqlone::qpalm::DetailedStats</class>
    <class kind="struct">cyqlone::qpalm::DetailedStats::Entry</class>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::qpalm</namespace>
  </compound>
  <compound kind="file">
    <name>algorithms.hpp</name>
    <path>src/qpalm/include/cyqlone/qpalm/implementation/</path>
    <filename>algorithms_8hpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::qpalm</namespace>
  </compound>
  <compound kind="file">
    <name>breakpoint.hpp</name>
    <path>src/qpalm/include/cyqlone/qpalm/implementation/</path>
    <filename>breakpoint_8hpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <includes id="neumaier_8hpp" name="neumaier.hpp" local="no" import="no" module="no" objc="no">cyqlone/neumaier.hpp</includes>
    <class kind="struct">cyqlone::qpalm::Breakpoint</class>
    <class kind="struct">cyqlone::qpalm::ABSums</class>
    <class kind="struct">cyqlone::qpalm::PartitionedBreakpoints</class>
    <class kind="struct">cyqlone::qpalm::BreakpointsResult</class>
    <class kind="struct">cyqlone::qpalm::compute_breakpoints_fn</class>
    <class kind="struct">cyqlone::qpalm::get_partitioned_breakpoints_fn</class>
    <class kind="struct">cyqlone::qpalm::get_breakpoints_fn</class>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::qpalm</namespace>
  </compound>
  <compound kind="file">
    <name>breakpoint.tpp</name>
    <path>src/qpalm/include/cyqlone/qpalm/implementation/</path>
    <filename>breakpoint_8tpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <includes id="breakpoint_8hpp" name="breakpoint.hpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/implementation/breakpoint.hpp</includes>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::qpalm</namespace>
  </compound>
  <compound kind="file">
    <name>linesearch.tpp</name>
    <path>src/qpalm/include/cyqlone/qpalm/backends/backend-cyqlone/</path>
    <filename>backends_2backend-cyqlone_2linesearch_8tpp.html</filename>
    <includes id="ocp-backend-cyqlone_8tpp" name="ocp-backend-cyqlone.tpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/backends/ocp-backend-cyqlone.tpp</includes>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::qpalm</namespace>
  </compound>
  <compound kind="file">
    <name>linesearch.tpp</name>
    <path>src/qpalm/include/cyqlone/qpalm/implementation/</path>
    <filename>implementation_2linesearch_8tpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <includes id="neumaier_8hpp" name="neumaier.hpp" local="no" import="no" module="no" objc="no">cyqlone/neumaier.hpp</includes>
    <includes id="algorithms_8hpp" name="algorithms.hpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/implementation/algorithms.hpp</includes>
    <includes id="breakpoint_8hpp" name="breakpoint.hpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/implementation/breakpoint.hpp</includes>
    <includes id="breakpoint_8tpp" name="breakpoint.tpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/implementation/breakpoint.tpp</includes>
    <class kind="struct">cyqlone::qpalm::LineSearchSettings</class>
    <class kind="struct">cyqlone::qpalm::LineSearch</class>
    <class kind="struct">cyqlone::qpalm::LineSearch::Result</class>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::qpalm</namespace>
    <member kind="define">
      <type>#define</type>
      <name>LINE_SEARCH_COMPARE_IMPLEMENTATIONS</name>
      <anchorfile>implementation_2linesearch_8tpp.html</anchorfile>
      <anchor>acd7da36899cf9c72a6805d3aa8d632c3</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>solver.tpp</name>
    <path>src/qpalm/include/cyqlone/qpalm/implementation/</path>
    <filename>solver_8tpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <includes id="detailed-stats_8hpp" name="detailed-stats.hpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/detailed-stats.hpp</includes>
    <includes id="implementation_2linesearch_8tpp" name="linesearch.tpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/implementation/linesearch.tpp</includes>
    <includes id="solver_8hpp" name="solver.hpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/solver.hpp</includes>
    <class kind="struct">cyqlone::qpalm::SolverImplementation</class>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::qpalm</namespace>
  </compound>
  <compound kind="file">
    <name>qpalm.hpp</name>
    <path>src/qpalm/include/cyqlone/qpalm/</path>
    <filename>qpalm_8hpp.html</filename>
  </compound>
  <compound kind="file">
    <name>settings.hpp</name>
    <path>src/qpalm/include/cyqlone/qpalm/</path>
    <filename>settings_8hpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <class kind="struct">cyqlone::qpalm::Settings</class>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::qpalm</namespace>
  </compound>
  <compound kind="file">
    <name>solver.hpp</name>
    <path>src/qpalm/include/cyqlone/qpalm/</path>
    <filename>solver_8hpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <includes id="detailed-stats_8hpp" name="detailed-stats.hpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/detailed-stats.hpp</includes>
    <includes id="settings_8hpp" name="settings.hpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/settings.hpp</includes>
    <includes id="status_8hpp" name="status.hpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/status.hpp</includes>
    <includes id="timing_8hpp" name="timing.hpp" local="no" import="no" module="no" objc="no">cyqlone/timing.hpp</includes>
    <class kind="struct">cyqlone::qpalm::SolverTimings</class>
    <class kind="struct">cyqlone::qpalm::SolverStats</class>
    <class kind="struct">cyqlone::qpalm::detail::backend_type&lt; std::unique_ptr&lt; T, D &gt; &gt;</class>
    <class kind="struct">cyqlone::qpalm::detail::backend_type&lt; T * &gt;</class>
    <class kind="class">cyqlone::qpalm::Solver</class>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::qpalm</namespace>
    <namespace>cyqlone::qpalm::detail</namespace>
  </compound>
  <compound kind="file">
    <name>status.hpp</name>
    <path>src/qpalm/include/cyqlone/qpalm/</path>
    <filename>status_8hpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::qpalm</namespace>
  </compound>
  <compound kind="file">
    <name>breakpoint.cpp</name>
    <path>src/qpalm/src/</path>
    <filename>breakpoint_8cpp.html</filename>
    <includes id="algorithms_8hpp" name="algorithms.hpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/implementation/algorithms.hpp</includes>
    <includes id="breakpoint_8hpp" name="breakpoint.hpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/implementation/breakpoint.hpp</includes>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::qpalm</namespace>
  </compound>
  <compound kind="file">
    <name>ocp-backend-cyqlone.cpp</name>
    <path>src/qpalm/src/</path>
    <filename>ocp-backend-cyqlone_8cpp.html</filename>
    <includes id="ocp-backend-cyqlone_8tpp" name="ocp-backend-cyqlone.tpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/backends/ocp-backend-cyqlone.tpp</includes>
    <includes id="solver_8tpp" name="solver.tpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/implementation/solver.tpp</includes>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::qpalm</namespace>
    <member kind="define">
      <type>#define</type>
      <name>CYQLONE_INSTANTIATE_QPALM_Cyqlone</name>
      <anchorfile>ocp-backend-cyqlone_8cpp.html</anchorfile>
      <anchor>a76a67ff6a8b41587d5f86389cd45e853</anchor>
      <arglist>(v, order)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>qpalm.cpp</name>
    <path>src/qpalm/src/</path>
    <filename>qpalm_8cpp.html</filename>
    <includes id="qpalm_8hpp" name="qpalm.hpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/qpalm.hpp</includes>
  </compound>
  <compound kind="file">
    <name>status.cpp</name>
    <path>src/qpalm/src/</path>
    <filename>status_8cpp.html</filename>
    <includes id="status_8hpp" name="status.hpp" local="no" import="no" module="no" objc="no">cyqlone/qpalm/status.hpp</includes>
    <namespace>cyqlone</namespace>
    <namespace>cyqlone::qpalm</namespace>
  </compound>
  <compound kind="file">
    <name>test-pcr.cpp</name>
    <path>test/</path>
    <filename>test-pcr_8cpp.html</filename>
    <includes id="config_8hpp" name="config.hpp" local="no" import="no" module="no" objc="no">cyqlone/config.hpp</includes>
    <includes id="linalg_8hpp" name="linalg.hpp" local="no" import="no" module="no" objc="no">cyqlone/linalg.hpp</includes>
    <includes id="matio_8hpp" name="matio.hpp" local="no" import="no" module="no" objc="no">cyqlone/matio.hpp</includes>
    <includes id="packing_8hpp" name="packing.hpp" local="no" import="no" module="no" objc="no">cyqlone/packing.hpp</includes>
    <class kind="struct">cyqlone::PCRFactorTest</class>
    <namespace>cyqlone</namespace>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>test-pcr_8cpp.html</anchorfile>
      <anchor>ae66f6b31b5ad750f1fe042a706a4e3d4</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::CyqloneParams</name>
    <filename>group__topic-ocp-solvers.html</filename>
    <anchor>structcyqlone_1_1CyqloneParams</anchor>
    <templarg>class T</templarg>
    <member kind="typedef">
      <type>T</type>
      <name>value_type</name>
      <anchorfile>group__topic-ocp-solvers.html</anchorfile>
      <anchor>a04be825e012f3109c62742ea3dd1a39c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::CyqloneSolver</name>
    <filename>structcyqlone_1_1CyqloneSolver.html</filename>
    <templarg>index_t VL</templarg>
    <templarg>class T</templarg>
    <templarg>StorageOrder DefaultOrder</templarg>
    <templarg>class Ctx</templarg>
    <member kind="typedef">
      <type>TricyqleSolver&lt; VL, T, DefaultOrder, Ctx &gt;</type>
      <name>tricyqle_t</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a93bf82649f3109b70c6da2ff8a8564d4</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>tricyqle_t::Context</type>
      <name>Context</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a70865e725fb07bd426601fdd106e4e93</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>tricyqle_t::SharedContext</type>
      <name>SharedContext</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>af277d1aa84ca404bd1b99e30cb3e70d9</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>tricyqle_t::simd</type>
      <name>simd</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a05fd2bf00fc3f6d6960e2b175d92beeb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const index_t</type>
      <name>p</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>adf8e80daae5cfdb1d12e0ba53352a545</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const index_t</type>
      <name>n</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a0002f525071b4f606369e0aa9518d798</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static constexpr index_t</type>
      <name>v</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>aefbb148acdcdbdfed8c15ffda0a06751</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>constexpr index_t</type>
      <name>lv</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a58c7a2be674a9fb4620c235d65e33020</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>constexpr index_t</type>
      <name>lp</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>aee01db15e6522e6a11e7e4e3dbf06b83</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>constexpr index_t</type>
      <name>ceil_p</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>acdf6900269bf5455d09918b3aaa7237d</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>constexpr index_t</type>
      <name>ceil_P</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a11b32cc8e77327e0adc134f512dc547f</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>std::unique_ptr&lt; SharedContext &gt;</type>
      <name>create_parallel_context</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a58cf7f941d02c0aee60d4e5872c6b104</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>foreach_stage</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a8b5b063b56c2df46df4262ed95d2c5ba</anchor>
      <arglist>(Context &amp;ctx, auto &amp;&amp;func, auto &amp;&amp;...xs) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>foreach_stage_fwd</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>acf4cdf325ef5ea8a3ba1145257d912ea</anchor>
      <arglist>(Context &amp;ctx, auto &amp;&amp;func, auto &amp;&amp;...xs) const</arglist>
    </member>
    <member kind="typedef">
      <type>typename tricyqle_t::template matrix&lt; O &gt;</type>
      <name>matrix</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a6c0c129cac2f6e09f5286e49f23a7c61</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename tricyqle_t::template view&lt; O &gt;</type>
      <name>view</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a1aad8ba69ab749ddff66b853f4829161</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename tricyqle_t::template mut_view&lt; O &gt;</type>
      <name>mut_view</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>ab686a13cd60338dfcde191e6e90faef6</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename tricyqle_t::layer_stride</type>
      <name>layer_stride</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a382638074af052c36681bf68249e58e2</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename tricyqle_t::template batch_view&lt; O &gt;</type>
      <name>batch_view</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a2d9149a288396c63a2c2a037293fddd1</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename tricyqle_t::template mut_batch_view&lt; O &gt;</type>
      <name>mut_batch_view</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a9f6452128a295c8f5717bfa2f5f810ef</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static constexpr auto</type>
      <name>default_order</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>ada5355929f47c994953d8a97c04838c2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static constexpr auto</type>
      <name>column_major</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>ad833cf9fa2805aa1ed20a0ac1e0b996f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const index_t</type>
      <name>N_horiz</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a6cf24ce87e31d5150b6bc07300bb5305</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const index_t</type>
      <name>nx</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>afc2380399f15aabcecc49b6584c638a9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const index_t</type>
      <name>nu</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>acae322d9f8f1e317e1652920714c97a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const index_t</type>
      <name>ny</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a831819545913b35b3d561747e67367a5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const index_t</type>
      <name>ny_0</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>abd0522443208690a953a9941e94a50c7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const index_t</type>
      <name>ny_N</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>aea90f9fa6fdefacce7edfa20d3102c7b</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>num_variables</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a01a76250369084fa7a2a8943ac51b98a</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>num_dynamics_constraints</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a7baf1f8c973ede51b874314a9daa870d</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>num_general_constraints</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>ae0fc6dd6f19f1ae9d0e1e815906412b4</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="variable">
      <type>CyqloneParams&lt; value_type &gt;</type>
      <name>params</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>ad659924e25e0ff23b735c9089421896f</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>CyqloneParams&lt; value_type &gt;</type>
      <name>get_params</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>ae8b404f03c9dd80e9cf79a7dda1a018f</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update_params</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a22127241427a5ab7ebfff0f6d0b19397</anchor>
      <arglist>(const CyqloneParams&lt; value_type &gt; &amp;new_params)</arglist>
    </member>
    <member kind="function">
      <type>TricyqleParams&lt; value_type &gt;</type>
      <name>get_tricyqle_params</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a5416083ca7e58a785f4ee880f1158393</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update_tricyqle_params</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>ac5781d5d0e8a01eb7b9b3a220629af7a</anchor>
      <arglist>(const TricyqleParams&lt; value_type &gt; &amp;new_params)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>get_params_string</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a5a6124994895dacc2e362147d20ef405</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="variable">
      <type>tricyqle_t</type>
      <name>tricyqle</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>ae9418afa01ce585ad9bd6854ce618e29</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix&lt; default_order &gt;</type>
      <name>data_H</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a07f9e7d22c1d6eb2e1bcc835c187e87a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix&lt; default_order &gt;</type>
      <name>data_F</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a1f040a73349b6834ba745932b300df1c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix&lt; default_order &gt;</type>
      <name>data_Gᵀ</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a2d59ea0eeee85dbf6acac690063f0100</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix&lt; default_order &gt;</type>
      <name>riccati_LH</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>acf7b3cce477b7f894e00da45bddae226</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix&lt; default_order &gt;</type>
      <name>riccati_LAB</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>afe924b77d1a04c1badb1c25614a1feb7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix&lt; default_order &gt;</type>
      <name>riccati_V</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>afbc5ec71c1e73f0f6cf3f54a911ee8b4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix&lt; column_major &gt;</type>
      <name>riccati_work</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>aa37613ea5c8f8aefe6991fc07ebdb05d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix&lt; column_major &gt;</type>
      <name>work_Σ</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>ae71853318a9651f71ad787d7c893da40</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix&lt; column_major &gt;</type>
      <name>riccati_Υ1</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a9f85302f40e3e95d1b6278e89dbf1bc1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix&lt; column_major &gt;</type>
      <name>riccati_Υ2</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a916a92730abd3583c01dfdcc375d6c9e</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>ceil_N</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a0db169898063877df567323139cb2ea3</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>ν2</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a493643e2a987e9ca6e0cdf655bd2f71e</anchor>
      <arglist>(index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>ν2p</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a1010a66b56be2a523d656c1dff7820ac</anchor>
      <arglist>(index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>add_wrap_ceil_N</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a191a6b691ea87e4e6b902153f2d2551e</anchor>
      <arglist>(index_t a, index_t b) const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>sub_wrap_ceil_N</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a55633cbae2421e6c366e3467656f19f1</anchor>
      <arglist>(index_t a, index_t b) const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>add_wrap_p</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a60736c59c456be402fe592463a21fe97</anchor>
      <arglist>(index_t a, index_t b) const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>sub_wrap_p</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>aa369a0232d0b7928ead986ffb96a45c3</anchor>
      <arglist>(index_t a, index_t b) const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>add_wrap_ceil_p</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>ab66988ba8add0f6f9956a301de8f706b</anchor>
      <arglist>(index_t a, index_t b) const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>sub_wrap_ceil_p</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a8bc67c9b88fa2d23bc5c2d33ebdf25e1</anchor>
      <arglist>(index_t a, index_t b) const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>sub_wrap_ceil_P</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>afaa87f8c8542e9ea247adef842e3a71b</anchor>
      <arglist>(index_t a, index_t b) const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>add_wrap_ceil_P</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a9915946dd4eb5f9510371eee719e0904</anchor>
      <arglist>(index_t a, index_t b) const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>get_linear_batch_offset</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a5bb754bbd276fa7d8b766e2ddaeb5126</anchor>
      <arglist>(index_t biA) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update_data</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a1b97601624c8e98a597cf61c790fbdae</anchor>
      <arglist>(const CyqloneStorage&lt; value_type &gt; &amp;ocp)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>initialize_rhs</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a8ca046a1255bcfbf774bcb3b81184537</anchor>
      <arglist>(const CyqloneStorage&lt; value_type &gt; &amp;ocp, mut_view&lt;&gt; rhs) const</arglist>
    </member>
    <member kind="function">
      <type>matrix</type>
      <name>initialize_rhs</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a428802da0172d474698bc4b406700f57</anchor>
      <arglist>(const CyqloneStorage&lt; value_type &gt; &amp;ocp) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>initialize_gradient</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a39590c5614a24539b5fc72b3a580f5dd</anchor>
      <arglist>(const CyqloneStorage&lt; value_type &gt; &amp;ocp, mut_view&lt;&gt; grad) const</arglist>
    </member>
    <member kind="function">
      <type>matrix</type>
      <name>initialize_gradient</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a9eede5c76d6b391ad867a1e4a7b907ab</anchor>
      <arglist>(const CyqloneStorage&lt; value_type &gt; &amp;ocp) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>initialize_bounds</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>aa6f5141dde78366e2baab1c2af0181b8</anchor>
      <arglist>(const CyqloneStorage&lt; value_type &gt; &amp;ocp, mut_view&lt;&gt; b_min, mut_view&lt;&gt; b_max) const</arglist>
    </member>
    <member kind="function">
      <type>std::pair&lt; matrix&lt;&gt;, matrix&lt;&gt; &gt;</type>
      <name>initialize_bounds</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>ad722a1be7eb9b9cb7b721df832e42e16</anchor>
      <arglist>(const CyqloneStorage&lt; value_type &gt; &amp;ocp) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>pack_variables</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a3342776dde10818e1885ada96d7dcb78</anchor>
      <arglist>(std::span&lt; const value_type &gt; ux_lin, mut_view&lt;&gt; ux) const</arglist>
    </member>
    <member kind="function">
      <type>matrix</type>
      <name>pack_variables</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>ae3d90ce0bede33a4c001a8e59813ffda</anchor>
      <arglist>(std::span&lt; const value_type &gt; ux_lin) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>unpack_variables</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a4093dc3807b5ab7f836452b63d716688</anchor>
      <arglist>(view&lt;&gt; ux, std::span&lt; value_type &gt; ux_lin) const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; value_type &gt;</type>
      <name>unpack_variables</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a931f627ce73af765c31e2358aa8581ca</anchor>
      <arglist>(view&lt;&gt; ux) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>pack_dynamics</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>abb037caa2933ed1bf02b2a93dd501bda</anchor>
      <arglist>(std::span&lt; const value_type &gt; λ_lin, mut_view&lt;&gt; λ) const</arglist>
    </member>
    <member kind="function">
      <type>matrix</type>
      <name>pack_dynamics</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>ac9d5a5b29182f296e15b3dab1f003db3</anchor>
      <arglist>(std::span&lt; const value_type &gt; λ_lin) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>unpack_dynamics</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>aef1a379e3430f7f145dbbc9f91819c70</anchor>
      <arglist>(view&lt;&gt; λ, std::span&lt; value_type &gt; λ_lin) const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; value_type &gt;</type>
      <name>unpack_dynamics</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>aabd815de7a359955fd53ff7b930cab3c</anchor>
      <arglist>(view&lt;&gt; λ) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>pack_constraints</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a362e27b672f651b08fc891df966498f3</anchor>
      <arglist>(std::span&lt; const value_type &gt; y_lin, mut_view&lt;&gt; y, value_type fill=0) const</arglist>
    </member>
    <member kind="function">
      <type>matrix</type>
      <name>pack_constraints</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a07a6c0541dc42e268a8e5eca5180b2dc</anchor>
      <arglist>(std::span&lt; const value_type &gt; y_lin, value_type fill=0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>unpack_constraints</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a5afd29f429ed17229de186c35d68a961</anchor>
      <arglist>(view&lt;&gt; y, std::span&lt; value_type &gt; y_lin) const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; value_type &gt;</type>
      <name>unpack_constraints</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a0de05df023944cb276da4dfb6aabad66</anchor>
      <arglist>(view&lt;&gt; y) const</arglist>
    </member>
    <member kind="function">
      <type>matrix</type>
      <name>initialize_variables</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a0c8bfede8d6baf4ebf8eff19b24d1f99</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>matrix</type>
      <name>initialize_dynamics_constraints</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a8580a763795b1243b7294aec5edd2e0b</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>matrix</type>
      <name>initialize_general_constraints</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a9d800cb6e4286943756e8ff9e8043a91</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static CyqloneSolver</type>
      <name>build</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a7f65a7abbe21c43f87ebd73e50724202</anchor>
      <arglist>(const CyqloneStorage&lt; value_type &gt; &amp;ocp, index_t p)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>residual_dynamics_constr</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a46cc003cdf64cad1f70f34d025d91677</anchor>
      <arglist>(Context &amp;ctx, view&lt;&gt; x, view&lt;&gt; b, mut_view&lt;&gt; Mxb) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>transposed_dynamics_constr</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a4646e385c850455c91650ec145d1d248</anchor>
      <arglist>(Context &amp;ctx, view&lt;&gt; λ, mut_view&lt;&gt; Mᵀλ, bool accum=false) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>general_constr</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a988fd0b1cf874daf030f172a02fe016e</anchor>
      <arglist>(Context &amp;ctx, view&lt;&gt; ux, mut_view&lt;&gt; DCux) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>transposed_general_constr</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a86359f82767432d004b6372614263215</anchor>
      <arglist>(Context &amp;ctx, view&lt;&gt; y, mut_view&lt;&gt; DCᵀy) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>transposed_general_constr</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a09957ff7eca0b1e3cfca52bd3fe52b9c</anchor>
      <arglist>(view&lt;&gt; y, mut_view&lt;&gt; DCᵀy) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cost_gradient</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a84b9120bc5d01ff549467f5b4f5fbc1f</anchor>
      <arglist>(Context &amp;ctx, view&lt;&gt; ux, value_type α, view&lt;&gt; q, value_type β, mut_view&lt;&gt; grad_f) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cost_gradient_regularized</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a682db0a07f3f638945a21f7aa581204c</anchor>
      <arglist>(Context &amp;ctx, value_type γ, view&lt;&gt; ux, view&lt;&gt; ux0, view&lt;&gt; q, mut_view&lt;&gt; grad_f) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cost_gradient_remove_regularization</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a39038aa297ee8d7e83f44d9ad34de4d6</anchor>
      <arglist>(Context &amp;ctx, value_type γ, view&lt;&gt; x, view&lt;&gt; x0, mut_view&lt;&gt; grad_f) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>factor_solve</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a3579cbcd843cefb7fe0ea3d7d8320e37</anchor>
      <arglist>(Context &amp;ctx, value_type γ, view&lt;&gt; Σ, mut_view&lt;&gt; ux, mut_view&lt;&gt; λ)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>factor</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a260bc6f10db3408569aa9948e1455516</anchor>
      <arglist>(Context &amp;ctx, value_type γ, view&lt;&gt; Σ)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve_forward</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a71528fddc44d7d5f9086c7a47cccb9d7</anchor>
      <arglist>(Context &amp;ctx, mut_view&lt;&gt; ux, mut_view&lt;&gt; λ)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve_reverse</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a9136ec0b4dc1e68df91ababe7199303c</anchor>
      <arglist>(Context &amp;ctx, mut_view&lt;&gt; ux, mut_view&lt;&gt; λ)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve_reverse_mul</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a2364dc5bde9db0e42b4a44bb4486c87e</anchor>
      <arglist>(Context &amp;ctx, mut_view&lt;&gt; ux, mut_view&lt;&gt; λ, mut_view&lt;&gt; Mᵀλ)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a07fd275ab569ba3ef98b65ef3ee118d5</anchor>
      <arglist>(Context &amp;ctx, view&lt;&gt; ΔΣ)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update_solve</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a325e41de017cac67c11bfc71bc532384</anchor>
      <arglist>(Context &amp;ctx, view&lt;&gt; ΔΣ, mut_view&lt;&gt; ux, mut_view&lt;&gt; λ)</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>riccati_thread_assignment</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a973e3bc711dcdb9067fbe1b66abfdf52</anchor>
      <arglist>(Context &amp;ctx) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>factor_riccati_solve</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>ad50a9c28a04298e88b898255f6a7e33b</anchor>
      <arglist>(Context &amp;ctx, value_type γ, view&lt;&gt; Σ, mut_view&lt;&gt; ux, mut_view&lt;&gt; λ)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>compute_schur</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a9b8b4a799aa454f96739baa065067e09</anchor>
      <arglist>(Context &amp;ctx, mut_view&lt;&gt; ux, mut_view&lt;&gt; λ)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>factor_solve_impl</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>aea826b013447afcbf77249bc8d480724</anchor>
      <arglist>(Context &amp;ctx, value_type γ, view&lt;&gt; Σ, mut_view&lt;&gt; ux, mut_view&lt;&gt; λ)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve_riccati_reverse</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a9434e9f657077ea619bd36055c87b1d8</anchor>
      <arglist>(Context &amp;ctx, mut_view&lt;&gt; ux, mut_view&lt;&gt; λ, mut_view&lt;&gt; work, std::optional&lt; mut_view&lt;&gt; &gt; Mᵀλ) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve_reverse</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a279e6c1efa690e277f83a3808a76e31e</anchor>
      <arglist>(Context &amp;ctx, mut_view&lt;&gt; ux, mut_view&lt;&gt; λ, mut_view&lt;&gt; work, std::optional&lt; mut_view&lt;&gt; &gt; Mᵀλ=std::nullopt) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update_riccati_solve</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>aa0b477c2d5d1ed2eedb27b9f8e003e90</anchor>
      <arglist>(Context &amp;ctx, view&lt;&gt; ΔΣ, mut_view&lt;&gt; ux, mut_view&lt;&gt; λ)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update_riccati</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a53a86302bd292d435b9f94b7efc66f96</anchor>
      <arglist>(Context &amp;ctx, view&lt;&gt; ΔΣ)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update_solve_impl</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a519b8669b31ad09015bfd5099691de27</anchor>
      <arglist>(Context &amp;ctx, view&lt;&gt; ΔΣ, mut_view&lt;&gt; ux, mut_view&lt;&gt; λ)</arglist>
    </member>
    <member kind="function">
      <type>SparseMatrix</type>
      <name>build_sparse</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a548e1ca2ae355c26d32d6a0a497bd0f7</anchor>
      <arglist>(const CyqloneStorage&lt; value_type &gt; &amp;ocp, std::span&lt; const value_type &gt; Σ) const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; value_type &gt;</type>
      <name>build_rhs</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>ae9a015047be9481d5e78394d7ce3b397</anchor>
      <arglist>(view&lt;&gt; rq, view&lt;&gt; b, value_type scale_rq=-1, value_type scale_b=-1) const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; value_type &gt;</type>
      <name>build_sol</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a027b0dd7eb90cf550363d1a2f6460407</anchor>
      <arglist>(view&lt;&gt; ux, view&lt;&gt; λ) const</arglist>
    </member>
    <member kind="function">
      <type>SparseMatrix</type>
      <name>build_sparse_factor</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>a07ad4a4aad9f82b0a327aafb3b4d5c92</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>SparseMatrix</type>
      <name>build_sparse_diag</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>ad5b252edc37cdcf3d6b026287a60bb68</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="typedef">
      <type>T</type>
      <name>value_type</name>
      <anchorfile>structcyqlone_1_1CyqloneSolver.html</anchorfile>
      <anchor>ad9abedeffb4a0678016b49c140fbda7e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::CyqloneStorage</name>
    <filename>structcyqlone_1_1CyqloneStorage.html</filename>
    <templarg>class T</templarg>
    <member kind="typedef">
      <type>T</type>
      <name>value_type</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>a8d6d781181e8cf5578bc6d8df6c4fa0c</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>batmat::matrix::Matrix&lt; value_type, index_t &gt;</type>
      <name>matrix</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>a461664ce662031ec80e24179d5aa160a</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>LinearOCPStorage::Solution</type>
      <name>Solution</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>a5077445bf9f6dd8b1d187db5eb8ecc20</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>LinearOCPStorage::KKTError</type>
      <name>KKTError</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>a8800a636bfb956a3750797b6849c82fb</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update_impl</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>a802bedcc1527e40bf4d0a73b532dd1a2</anchor>
      <arglist>(const LinearOCPStorage &amp;ocp)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>aa2b03be361da3dc445fbe12a7f5b14e4</anchor>
      <arglist>(const LinearOCPStorage &amp;ocp)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reconstruct_ineq_multipliers</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>a67496e3ee37459cfd9fa6d9f98c70a5e</anchor>
      <arglist>(std::span&lt; const value_type &gt; y_compressed, std::span&lt; value_type &gt; y) const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; value_type &gt;</type>
      <name>reconstruct_ineq_multipliers</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>a43c82f3ed7f435db9b028baba5419b91</anchor>
      <arglist>(std::span&lt; const value_type &gt; y_compressed) const</arglist>
    </member>
    <member kind="function">
      <type>Solution</type>
      <name>reconstruct_solution</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>a2c090e2365a930356da9667415e10bb3</anchor>
      <arglist>(const LinearOCPStorage &amp;ocp, std::span&lt; const value_type &gt; ux_compressed, std::span&lt; const value_type &gt; y_compressed, std::span&lt; const value_type &gt; λ_compressed) const</arglist>
    </member>
    <member kind="function">
      <type>KKTError</type>
      <name>compute_kkt_error</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>ad01b4d57231a32421e798a03be313310</anchor>
      <arglist>(const LinearOCPStorage &amp;ocp, std::span&lt; const value_type &gt; ux_compressed, std::span&lt; const value_type &gt; y_compressed, std::span&lt; const value_type &gt; λ_compressed) const</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static CyqloneStorage</type>
      <name>build</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>afebcc052a5712f3998178d3583d00f1c</anchor>
      <arglist>(const LinearOCPStorage &amp;ocp, index_t ny_0=-1)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static index_t</type>
      <name>count_constr_0</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>a128cfe523b35c66b0f292e75e027ba11</anchor>
      <arglist>(const LinearOCPStorage &amp;ocp, std::vector&lt; bool &gt; &amp;Ju0)</arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>N_horiz</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>a7eca197b9842281035693dfba5240da4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>nx</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>a2a7e2b890c3d6c8feabb811aa7c9945b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>nu</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>a94f6e3df359a642db24a1314c5d6b834</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>ny</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>aa105440032c8ead7014d78478855f559</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>ny_0</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>adcdc3f7d303534083908cc9e5efe331c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>ny_N</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>ac6c84b75bc758b0817e06d20d33d3dc0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; bool &gt;</type>
      <name>Ju0</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>ae49ba7578862d01d9d74b0393da36449</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix</type>
      <name>data_H</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>ae59a1ad04812a951b67cffb555e2c46e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix</type>
      <name>data_F</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>a9858e75b9e77fa90942ae9e93bee6cd0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix</type>
      <name>data_G</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>ac42f16fa5afd1539c75464abf5224c2e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix</type>
      <name>data_G0N</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>a9d576ab731437a666d1779180ab0da93</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix</type>
      <name>data_rq</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>a210cf14825f736136173613ccba6e5a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix</type>
      <name>data_c</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>a0f2e51a560de04b725a2835530c9d6c1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix</type>
      <name>data_lb</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>a39945295b599e4e6eb8603d4545fbb01</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix</type>
      <name>data_lb0N</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>af7d6268a019a69170affc90f45f6fcbb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix</type>
      <name>data_ub</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>a02ff2708ce8de1cd931128a1f22c4de6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix</type>
      <name>data_ub0N</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>a9a940b82db836d321c2b85af56718c26</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; index_t &gt;</type>
      <name>indices_G0</name>
      <anchorfile>structcyqlone_1_1CyqloneStorage.html</anchorfile>
      <anchor>a3a48efdcefe05de53bbf06d911ac2411</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::EmptyCompletion</name>
    <filename>structcyqlone_1_1EmptyCompletion.html</filename>
    <member kind="function">
      <type>void</type>
      <name>operator()</name>
      <anchorfile>structcyqlone_1_1EmptyCompletion.html</anchorfile>
      <anchor>ae966a81a50062589cf052e3173e87555</anchor>
      <arglist>() const noexcept</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::LinearOCPSparseQP</name>
    <filename>structcyqlone_1_1LinearOCPSparseQP.html</filename>
    <member kind="function">
      <type>KKTMatrix</type>
      <name>build_kkt</name>
      <anchorfile>structcyqlone_1_1LinearOCPSparseQP.html</anchorfile>
      <anchor>ae9ee3917664f7a4ec2551c7c31ea8851</anchor>
      <arglist>(real_t S, std::span&lt; const real_t &gt; Σ, std::span&lt; const bool &gt; J) const</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static LinearOCPSparseQP</type>
      <name>build</name>
      <anchorfile>structcyqlone_1_1LinearOCPSparseQP.html</anchorfile>
      <anchor>a4c173a4c959b79f6a3bb82b29b4351c6</anchor>
      <arglist>(const LinearOCPStorage &amp;ocp)</arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; index_t &gt;</type>
      <name>Q_outer_ptr</name>
      <anchorfile>structcyqlone_1_1LinearOCPSparseQP.html</anchorfile>
      <anchor>aa853d04b50ecfd335c6e5c57c31188db</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; index_t &gt;</type>
      <name>Q_inner_idx</name>
      <anchorfile>structcyqlone_1_1LinearOCPSparseQP.html</anchorfile>
      <anchor>ad14fd39b30a87b5aa11ebae22bc0c0df</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; real_t &gt;</type>
      <name>Q_values</name>
      <anchorfile>structcyqlone_1_1LinearOCPSparseQP.html</anchorfile>
      <anchor>af8de074f5f4d602a788448bfc2fd0b36</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>SparseCSC&lt; index_t, index_t &gt;</type>
      <name>Q_sparsity</name>
      <anchorfile>structcyqlone_1_1LinearOCPSparseQP.html</anchorfile>
      <anchor>a8dc6e2385fb4b96a22739e2c1bb0f786</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; index_t &gt;</type>
      <name>A_outer_ptr</name>
      <anchorfile>structcyqlone_1_1LinearOCPSparseQP.html</anchorfile>
      <anchor>a41899c587babe5186af00a8a36524271</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; index_t &gt;</type>
      <name>A_inner_idx</name>
      <anchorfile>structcyqlone_1_1LinearOCPSparseQP.html</anchorfile>
      <anchor>a6614256b4762efc491821cbf2eb6a8a2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; real_t &gt;</type>
      <name>A_values</name>
      <anchorfile>structcyqlone_1_1LinearOCPSparseQP.html</anchorfile>
      <anchor>a3a1b6f7b21b3ad1fef1bb5d6e1c93c79</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>SparseCSC&lt; index_t, index_t &gt;</type>
      <name>A_sparsity</name>
      <anchorfile>structcyqlone_1_1LinearOCPSparseQP.html</anchorfile>
      <anchor>a0edc88c0e3c809e3fed970b03faee232</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>n</name>
      <anchorfile>structcyqlone_1_1LinearOCPSparseQP.html</anchorfile>
      <anchor>a71916e1a99b3913b68c32ec91cf26c89</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>m_eq</name>
      <anchorfile>structcyqlone_1_1LinearOCPSparseQP.html</anchorfile>
      <anchor>a245d1d336c4b4d926aaffe1c23fad9b7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>m_ineq</name>
      <anchorfile>structcyqlone_1_1LinearOCPSparseQP.html</anchorfile>
      <anchor>a8c61d6796749c4ebfb43dcc95df0334c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::LinearOCPSparseQP::KKTMatrix</name>
    <filename>structcyqlone_1_1LinearOCPSparseQP.html</filename>
    <anchor>structcyqlone_1_1LinearOCPSparseQP_1_1KKTMatrix</anchor>
    <member kind="variable">
      <type>std::vector&lt; index_t &gt;</type>
      <name>outer_ptr</name>
      <anchorfile>structcyqlone_1_1LinearOCPSparseQP.html</anchorfile>
      <anchor>a56c5bcab8e01759d78424ffe138ee938</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; index_t &gt;</type>
      <name>inner_idx</name>
      <anchorfile>structcyqlone_1_1LinearOCPSparseQP.html</anchorfile>
      <anchor>a8b4a204db97df41e3d002d296368ed66</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; real_t &gt;</type>
      <name>values</name>
      <anchorfile>structcyqlone_1_1LinearOCPSparseQP.html</anchorfile>
      <anchor>a0ce9b4ea58045a7a4298f6442bb6e767</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>SparseCSC&lt; index_t, index_t &gt;</type>
      <name>sparsity</name>
      <anchorfile>structcyqlone_1_1LinearOCPSparseQP.html</anchorfile>
      <anchor>adbdd8d3223597f63001ec9d9866feba5</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::LinearOCPStorage</name>
    <filename>structcyqlone_1_1LinearOCPStorage.html</filename>
    <class kind="struct">cyqlone::LinearOCPStorage::Solution</class>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>H</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a59af5d28ff7e416d02a97f2ed80bfbe3</anchor>
      <arglist>(index_t i)</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>Q</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a7d67863311c04bf6d04dff4ffc0501a8</anchor>
      <arglist>(index_t i)</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>R</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a4b6c769284d0c9f39a0d799b1455d154</anchor>
      <arglist>(index_t i)</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>S</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a7d1057eb3b8246f290f314d5124948cc</anchor>
      <arglist>(index_t i)</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>S_trans</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a84859ccb81131e7a5bc78e2bada07cf4</anchor>
      <arglist>(index_t i)</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>CD</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a2d7427bf59ec58926b372466a082066a</anchor>
      <arglist>(index_t i)</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>C</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a7aefb81825e44d5526b906ca37d66c23</anchor>
      <arglist>(index_t i)</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>D</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a3287504c95654a514de8513e2b21f582</anchor>
      <arglist>(index_t i)</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>AB</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>aaece1976d63161b367c0c755f576774e</anchor>
      <arglist>(index_t i)</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>A</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a80e92e086228f987316f1c8a46d1a213</anchor>
      <arglist>(index_t i)</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>B</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a2a240d539848ef1b9cdb68ce56f172bf</anchor>
      <arglist>(index_t i)</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>H</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a64c07d11eb851d4d5a930521a30949f8</anchor>
      <arglist>(index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>Q</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>afec56db3e879ff34cae2fc2f954fd243</anchor>
      <arglist>(index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>R</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>ac1af58a300b10128dc38647c21008874</anchor>
      <arglist>(index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>S</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a505571e8071bd5342bf34e8dced7cd2f</anchor>
      <arglist>(index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>S_trans</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>aaa9e756091a07ec76fa0300c77ae67f6</anchor>
      <arglist>(index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>CD</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a52a9956d36c16eba2692383937c785b1</anchor>
      <arglist>(index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>C</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>ab3f086541ed2e71626f68e4ec4a6d49d</anchor>
      <arglist>(index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>D</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a6e80ef9ed9fafd8550220cdd6f2037ee</anchor>
      <arglist>(index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>AB</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>ae99e0b7a571b2478b9c1ba110cc9987c</anchor>
      <arglist>(index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>A</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a27d61e750b9e9555d1044a3b301c7d37</anchor>
      <arglist>(index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>B</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a698af7f1cb6918a0bb80c08f36e15c9c</anchor>
      <arglist>(index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>num_variables</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>acdc3b51f3656b50266bacb98751c547b</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>num_constraints</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a8a2352b8f9754dde69173d4de522181c</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>num_dynamics_constraints</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>aa1b329745b5c682f335cff88c8c138ad</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>qr</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>ad245c03b13212aaa2289c075db5896cd</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>qr</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a1c2b1370ce21a0bde81419000e878848</anchor>
      <arglist>(index_t i)</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>q</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a18987fd40bfbf98512ea0a831184fceb</anchor>
      <arglist>(index_t i)</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>r</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a00cf31fb617a4375e87e3bc94b2c023e</anchor>
      <arglist>(index_t i)</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>b</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a48a1dec77ab4f51bf1e4f179fa63299b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>b</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>ad66945575e7fca5f8454f273bffb0ee4</anchor>
      <arglist>(index_t i)</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>b_min</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a403b4636df1f0e24a1c3e69b9f956513</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>b_min</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>aaa0f7456157a720cef41973d04a8474d</anchor>
      <arglist>(index_t i)</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>b_max</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a879debd68bbc90316317761550b5b797</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>b_max</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>afac7110d732c43f85010408d16e7f27f</anchor>
      <arglist>(index_t i)</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>qr</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>adac176f7c73d80b07e97991daab86299</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>qr</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>ac1b125944a15698658a99f2d38ed44a8</anchor>
      <arglist>(index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>q</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a58f77ebdcb6b8cf6fa6500ba66789b55</anchor>
      <arglist>(index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>r</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>ac49eab2351c2727fc65901bcaef25634</anchor>
      <arglist>(index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>b</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a89d340ab09b81625dbc99f78de5a9643</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>b</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>ad8db594998e23cbbe90cec64d8219ca7</anchor>
      <arglist>(index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>b_min</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a72a94dd852aea524c1b02f1d5ac841ed</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>b_min</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>af25b911a98ccd90a688645bcc80c84fb</anchor>
      <arglist>(index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>b_max</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>ab95d280f08c51e2339c29c34bb40fb42</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>b_max</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a24e5e12fd63f149c0bb67af7f5544037</anchor>
      <arglist>(index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>KKTError</type>
      <name>compute_kkt_error</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a477befd0b7ebf9341d399fa43d4e6176</anchor>
      <arglist>(const Solution &amp;sol) const</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static std::vector&lt; real_t &gt;</type>
      <name>create_storage</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a6ba4f8f1df4ac6636d3009c5791fbfa7</anchor>
      <arglist>(OCPDim dim)</arglist>
    </member>
    <member kind="variable">
      <type>OCPDim</type>
      <name>dim</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>af6e9e4a08238975565a9f2769e5fc121</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; real_t &gt;</type>
      <name>storage</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a1e392bacae8af0e27b96c08037b7aec8</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::LinearOCPStorage::KKTError</name>
    <filename>structcyqlone_1_1LinearOCPStorage.html</filename>
    <anchor>structcyqlone_1_1LinearOCPStorage_1_1KKTError</anchor>
    <member kind="variable">
      <type>real_t</type>
      <name>stationarity</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a24dd4b50f7770134875f10d21fb5a736</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>inequality_residual</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>afd463cff91741360c951fa34dbd2dc01</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>equality_residual</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>a2e0836a2b4627eafeee13186ad0d504a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>complementarity</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage.html</anchorfile>
      <anchor>aec340cd6313ecc032c6cd1b88b41d89b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::LinearOCPStorage::Solution</name>
    <filename>structcyqlone_1_1LinearOCPStorage_1_1Solution.html</filename>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>x</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage_1_1Solution.html</anchorfile>
      <anchor>a425c2d4bb9e7dcd53037df08727d5136</anchor>
      <arglist>(OCPDim dim)</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>x</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage_1_1Solution.html</anchorfile>
      <anchor>a52a40c70f02131892376d28fbe697918</anchor>
      <arglist>(OCPDim dim) const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>x</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage_1_1Solution.html</anchorfile>
      <anchor>a503548b931fe97fd871677e77c713644</anchor>
      <arglist>(OCPDim dim, index_t i)</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>x</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage_1_1Solution.html</anchorfile>
      <anchor>ac85fac03cc34432f60c0c515d41c04b0</anchor>
      <arglist>(OCPDim dim, index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>u</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage_1_1Solution.html</anchorfile>
      <anchor>a044e078de78960184f902efdfd6ce595</anchor>
      <arglist>(OCPDim dim)</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>u</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage_1_1Solution.html</anchorfile>
      <anchor>a909337e8f2f7ef1bb34ad6ff0f495cf1</anchor>
      <arglist>(OCPDim dim) const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>u</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage_1_1Solution.html</anchorfile>
      <anchor>a2ba673966e3f39ae3c0855ce704fccda</anchor>
      <arglist>(OCPDim dim, index_t i)</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>u</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage_1_1Solution.html</anchorfile>
      <anchor>aed0aed3c97c95b98d46e70c3a0ec8def</anchor>
      <arglist>(OCPDim dim, index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>λ</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage_1_1Solution.html</anchorfile>
      <anchor>ac83413181846e0d2f8b916fb8b40a463</anchor>
      <arglist>(OCPDim dim)</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>λ</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage_1_1Solution.html</anchorfile>
      <anchor>aed5345c2f09e35727671a4dcc156fec0</anchor>
      <arglist>(OCPDim dim) const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>λ</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage_1_1Solution.html</anchorfile>
      <anchor>a993e894897541ab5b71b0cd60ce9b502</anchor>
      <arglist>(OCPDim dim, index_t i)</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>λ</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage_1_1Solution.html</anchorfile>
      <anchor>af926d24915f29acde923176de7e598f9</anchor>
      <arglist>(OCPDim dim, index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; real_t, index_t &gt;</type>
      <name>y</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage_1_1Solution.html</anchorfile>
      <anchor>a464d8ebc2fe1a51b00ba3fefc538494b</anchor>
      <arglist>(OCPDim dim, index_t i)</arglist>
    </member>
    <member kind="function">
      <type>guanaqo::MatrixView&lt; const real_t, index_t &gt;</type>
      <name>y</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage_1_1Solution.html</anchorfile>
      <anchor>a30a55a890a025eaa4ca6d45dc60266d1</anchor>
      <arglist>(OCPDim dim, index_t i) const</arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; real_t &gt;</type>
      <name>solution</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage_1_1Solution.html</anchorfile>
      <anchor>a3ea90dd6d6b601c9c919ab2edb251b7d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; real_t &gt;</type>
      <name>inequality_multipliers</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage_1_1Solution.html</anchorfile>
      <anchor>a27048ba2583ea8247b4d5dc35d5f7038</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; real_t &gt;</type>
      <name>equality_multipliers</name>
      <anchorfile>structcyqlone_1_1LinearOCPStorage_1_1Solution.html</anchorfile>
      <anchor>aa187d42123b8a9ea7d9d6d9044db4844</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::matio_traits</name>
    <filename>namespacecyqlone.html</filename>
    <anchor>structcyqlone_1_1matio__traits</anchor>
    <templarg>class T</templarg>
  </compound>
  <compound kind="struct">
    <name>cyqlone::matio_traits&lt; double &gt;</name>
    <filename>structcyqlone_1_1matio__traits_3_01double_01_4.html</filename>
    <member kind="variable" static="yes">
      <type>static constexpr auto</type>
      <name>type</name>
      <anchorfile>structcyqlone_1_1matio__traits_3_01double_01_4.html</anchorfile>
      <anchor>aa4776a663dc80c88f71b89f86983e459</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static constexpr auto</type>
      <name>class_</name>
      <anchorfile>structcyqlone_1_1matio__traits_3_01double_01_4.html</anchorfile>
      <anchor>a8a84bf60e7b2a7652a356fc42efe706b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::matio_traits&lt; float &gt;</name>
    <filename>structcyqlone_1_1matio__traits_3_01float_01_4.html</filename>
    <member kind="variable" static="yes">
      <type>static constexpr auto</type>
      <name>type</name>
      <anchorfile>structcyqlone_1_1matio__traits_3_01float_01_4.html</anchorfile>
      <anchor>a4318c17d379f9d3b7124b8dc169359c8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static constexpr auto</type>
      <name>class_</name>
      <anchorfile>structcyqlone_1_1matio__traits_3_01float_01_4.html</anchorfile>
      <anchor>a04c58f1d5de22db170345c82cadd497b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::matio_traits&lt; I &gt;</name>
    <filename>structcyqlone_1_1matio__traits_3_01I_01_4.html</filename>
    <templarg>std::unsigned_integral I</templarg>
    <member kind="variable" static="yes">
      <type>static constexpr auto</type>
      <name>type</name>
      <anchorfile>structcyqlone_1_1matio__traits_3_01I_01_4.html</anchorfile>
      <anchor>a10ae717db2e7b1bd20fdcd50b4ddcb82</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static constexpr auto</type>
      <name>class_</name>
      <anchorfile>structcyqlone_1_1matio__traits_3_01I_01_4.html</anchorfile>
      <anchor>a2bbc49ee7419c9c28cb7529240af5e41</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>cyqlone::NeumaierSum</name>
    <filename>classcyqlone_1_1NeumaierSum.html</filename>
    <templarg>class T</templarg>
    <member kind="function">
      <type></type>
      <name>NeumaierSum</name>
      <anchorfile>classcyqlone_1_1NeumaierSum.html</anchorfile>
      <anchor>a023246a8890b36f4196b2dfe7a6d40d6</anchor>
      <arglist>(T sum, T compensation)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>NeumaierSum</name>
      <anchorfile>classcyqlone_1_1NeumaierSum.html</anchorfile>
      <anchor>a7eae35f0eb6ff40578eb75bd3d0b97d8</anchor>
      <arglist>(T value={})</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>operator T</name>
      <anchorfile>classcyqlone_1_1NeumaierSum.html</anchorfile>
      <anchor>a1e9d8e69fbe7fb21a7d22222ec32fa4c</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>NeumaierSum</type>
      <name>operator-</name>
      <anchorfile>classcyqlone_1_1NeumaierSum.html</anchorfile>
      <anchor>af4a486126bb2f5861efc3c36596de62f</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>NeumaierSum &amp;</type>
      <name>operator+=</name>
      <anchorfile>classcyqlone_1_1NeumaierSum.html</anchorfile>
      <anchor>a7c9a9a7e0d20f1a26be61b3c1ac62913</anchor>
      <arglist>(T v)</arglist>
    </member>
    <member kind="function">
      <type>NeumaierSum &amp;</type>
      <name>operator+=</name>
      <anchorfile>classcyqlone_1_1NeumaierSum.html</anchorfile>
      <anchor>adfdfea95f2a07dcc5b5172bd86fb178e</anchor>
      <arglist>(const NeumaierSum &amp;other)</arglist>
    </member>
    <member kind="function">
      <type>NeumaierSum &amp;</type>
      <name>operator-=</name>
      <anchorfile>classcyqlone_1_1NeumaierSum.html</anchorfile>
      <anchor>ae0827ceb18a50a2352479817e15ea71c</anchor>
      <arglist>(T v)</arglist>
    </member>
    <member kind="function">
      <type>NeumaierSum &amp;</type>
      <name>operator-=</name>
      <anchorfile>classcyqlone_1_1NeumaierSum.html</anchorfile>
      <anchor>ae2e1be6f876d0e24d67353b9d1205136</anchor>
      <arglist>(const NeumaierSum &amp;other)</arglist>
    </member>
    <member kind="variable">
      <type>T</type>
      <name>sum</name>
      <anchorfile>classcyqlone_1_1NeumaierSum.html</anchorfile>
      <anchor>aadc3d20324c0a51493451fdeb3e1f411</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>T</type>
      <name>compensation</name>
      <anchorfile>classcyqlone_1_1NeumaierSum.html</anchorfile>
      <anchor>a7ca4d34bf91570c1a170e9b12ae099d3</anchor>
      <arglist></arglist>
    </member>
    <member kind="friend">
      <type>friend NeumaierSum</type>
      <name>operator+</name>
      <anchorfile>classcyqlone_1_1NeumaierSum.html</anchorfile>
      <anchor>aa1a0ff64fbd206621f3932fd3d889cbd</anchor>
      <arglist>(NeumaierSum lhs, T rhs)</arglist>
    </member>
    <member kind="friend">
      <type>friend NeumaierSum</type>
      <name>operator+</name>
      <anchorfile>classcyqlone_1_1NeumaierSum.html</anchorfile>
      <anchor>a79b2b0a27d953a81fd12901e4aa92482</anchor>
      <arglist>(T lhs, NeumaierSum rhs)</arglist>
    </member>
    <member kind="friend">
      <type>friend NeumaierSum</type>
      <name>operator+</name>
      <anchorfile>classcyqlone_1_1NeumaierSum.html</anchorfile>
      <anchor>a81cc781ba392c62870490923aec1b788</anchor>
      <arglist>(NeumaierSum lhs, NeumaierSum rhs)</arglist>
    </member>
    <member kind="friend">
      <type>friend NeumaierSum</type>
      <name>operator-</name>
      <anchorfile>classcyqlone_1_1NeumaierSum.html</anchorfile>
      <anchor>a7489c5f2d5249961c8edca3e95be9b30</anchor>
      <arglist>(NeumaierSum lhs, T rhs)</arglist>
    </member>
    <member kind="friend">
      <type>friend NeumaierSum</type>
      <name>operator-</name>
      <anchorfile>classcyqlone_1_1NeumaierSum.html</anchorfile>
      <anchor>a561f4df562069d61ae54b6fc1e5e5ede</anchor>
      <arglist>(T lhs, NeumaierSum rhs)</arglist>
    </member>
    <member kind="friend">
      <type>friend NeumaierSum</type>
      <name>operator-</name>
      <anchorfile>classcyqlone_1_1NeumaierSum.html</anchorfile>
      <anchor>a7f28fe9f7125430a30e6a086a5812bd7</anchor>
      <arglist>(NeumaierSum lhs, const NeumaierSum &amp;rhs)</arglist>
    </member>
    <member kind="friend">
      <type>friend NeumaierSum</type>
      <name>operator*</name>
      <anchorfile>classcyqlone_1_1NeumaierSum.html</anchorfile>
      <anchor>a27ec7543c36110a2281af26a9bedd8e5</anchor>
      <arglist>(NeumaierSum lhs, NeumaierSum rhs)</arglist>
    </member>
    <member kind="friend">
      <type>friend NeumaierSum</type>
      <name>operator*</name>
      <anchorfile>classcyqlone_1_1NeumaierSum.html</anchorfile>
      <anchor>a39555a9263f53e921f6606941f7325fe</anchor>
      <arglist>(NeumaierSum lhs, T rhs)</arglist>
    </member>
    <member kind="friend">
      <type>friend NeumaierSum</type>
      <name>operator*</name>
      <anchorfile>classcyqlone_1_1NeumaierSum.html</anchorfile>
      <anchor>a78ba89c6c79ffd4b81408693c4b47171</anchor>
      <arglist>(T lhs, NeumaierSum rhs)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::norms</name>
    <filename>structcyqlone_1_1norms.html</filename>
    <templarg>class T</templarg>
    <templarg>class simd</templarg>
    <member kind="typedef">
      <type>typename norms&lt; T &gt;::result</type>
      <name>result</name>
      <anchorfile>structcyqlone_1_1norms.html</anchorfile>
      <anchor>acd004182a7328d96fffa3c56a019c3a5</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>result_simd</type>
      <name>operator()</name>
      <anchorfile>structcyqlone_1_1norms.html</anchorfile>
      <anchor>a5cddc1c0b1676fd0b22f60a3638a0caf</anchor>
      <arglist>(result_simd accum, simd t) const</arglist>
    </member>
    <member kind="function">
      <type>result</type>
      <name>operator()</name>
      <anchorfile>structcyqlone_1_1norms.html</anchorfile>
      <anchor>a312d14b1e7e5274be8e6320d76ed33f4</anchor>
      <arglist>(result_simd accum) const</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static result_simd</type>
      <name>zero_simd</name>
      <anchorfile>structcyqlone_1_1norms.html</anchorfile>
      <anchor>a1fa0c63b386ad6fa5766870f1fb34eae</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::norms::result_simd</name>
    <filename>structcyqlone_1_1norms.html</filename>
    <anchor>structcyqlone_1_1norms_1_1result__simd</anchor>
    <member kind="variable">
      <type>simd</type>
      <name>amax</name>
      <anchorfile>structcyqlone_1_1norms.html</anchorfile>
      <anchor>a4df711757b01f11eedfa2cee42e241b8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>simd</type>
      <name>asum</name>
      <anchorfile>structcyqlone_1_1norms.html</anchorfile>
      <anchor>a1f39de7cc82aa5ce08d13fb0b0c4a162</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>simd</type>
      <name>sumsq</name>
      <anchorfile>structcyqlone_1_1norms.html</anchorfile>
      <anchor>a9e93f56b0ed66e8942e571b1769b6ebb</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::norms&lt; T, void &gt;</name>
    <filename>structcyqlone_1_1norms_3_01T_00_01void_01_4.html</filename>
    <templarg>class T</templarg>
    <class kind="struct">cyqlone::norms&lt; T, void &gt;::result</class>
    <member kind="typedef">
      <type>typename norms&lt; T &gt;::result</type>
      <name>result</name>
      <anchorfile>structcyqlone_1_1norms.html</anchorfile>
      <anchor>acd004182a7328d96fffa3c56a019c3a5</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>result</type>
      <name>operator()</name>
      <anchorfile>structcyqlone_1_1norms_3_01T_00_01void_01_4.html</anchorfile>
      <anchor>a53d8853b933dc9a4ae19c612cb6d8ce1</anchor>
      <arglist>(result accum, T t) const</arglist>
    </member>
    <member kind="function">
      <type>result</type>
      <name>operator()</name>
      <anchorfile>structcyqlone_1_1norms_3_01T_00_01void_01_4.html</anchorfile>
      <anchor>a08cc788d323708afc55d14ca1cc7957e</anchor>
      <arglist>(result accum, result t) const</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static result</type>
      <name>zero</name>
      <anchorfile>structcyqlone_1_1norms_3_01T_00_01void_01_4.html</anchorfile>
      <anchor>a91bbc77fcca70720a112b354c55e5cb4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static result_simd</type>
      <name>zero_simd</name>
      <anchorfile>structcyqlone_1_1norms.html</anchorfile>
      <anchor>a1fa0c63b386ad6fa5766870f1fb34eae</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::norms&lt; T, void &gt;::result</name>
    <filename>structcyqlone_1_1norms_3_01T_00_01void_01_4_1_1result.html</filename>
    <member kind="function">
      <type>T</type>
      <name>norm_1</name>
      <anchorfile>structcyqlone_1_1norms_3_01T_00_01void_01_4_1_1result.html</anchorfile>
      <anchor>a9e4b2f0ccab08aaca1d0b64830530699</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>norm_2</name>
      <anchorfile>structcyqlone_1_1norms_3_01T_00_01void_01_4_1_1result.html</anchorfile>
      <anchor>a6607d6e3206400cd69de845523e9b859</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>norm_inf</name>
      <anchorfile>structcyqlone_1_1norms_3_01T_00_01void_01_4_1_1result.html</anchorfile>
      <anchor>af6d8423fe66d5da13575c8c19715b831</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="variable">
      <type>T</type>
      <name>amax</name>
      <anchorfile>structcyqlone_1_1norms_3_01T_00_01void_01_4_1_1result.html</anchorfile>
      <anchor>a5dc0f728ca5a884e644e44bebd624670</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>T</type>
      <name>asum</name>
      <anchorfile>structcyqlone_1_1norms_3_01T_00_01void_01_4_1_1result.html</anchorfile>
      <anchor>a8c35129719e7462be73debf4f7cac49e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>T</type>
      <name>sumsq</name>
      <anchorfile>structcyqlone_1_1norms_3_01T_00_01void_01_4_1_1result.html</anchorfile>
      <anchor>a97310921bb4629f482b78eaef9f16aa2</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::OCPDim</name>
    <filename>structcyqlone_1_1OCPDim.html</filename>
    <member kind="variable">
      <type>index_t</type>
      <name>N_horiz</name>
      <anchorfile>structcyqlone_1_1OCPDim.html</anchorfile>
      <anchor>af0d2cff4cc9ab2b414ca90a4e58ce089</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>nx</name>
      <anchorfile>structcyqlone_1_1OCPDim.html</anchorfile>
      <anchor>a54587b454b1ad43b73db4c3f3cd2873d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>nu</name>
      <anchorfile>structcyqlone_1_1OCPDim.html</anchorfile>
      <anchor>a2609e1bcd727d61e5b66401077daf2f4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>ny</name>
      <anchorfile>structcyqlone_1_1OCPDim.html</anchorfile>
      <anchor>a1fc89a4bacdd348d614d0940679f176c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>ny_N</name>
      <anchorfile>structcyqlone_1_1OCPDim.html</anchorfile>
      <anchor>a294b9f592b558363999f15d77d74a2f3</anchor>
      <arglist></arglist>
    </member>
    <member kind="friend">
      <type>friend constexpr bool</type>
      <name>operator==</name>
      <anchorfile>structcyqlone_1_1OCPDim.html</anchorfile>
      <anchor>a4511f2022217e0599d1d2bc20054f1c9</anchor>
      <arglist>(OCPDim, OCPDim)=default</arglist>
    </member>
    <member kind="friend">
      <type>friend constexpr bool</type>
      <name>operator!=</name>
      <anchorfile>structcyqlone_1_1OCPDim.html</anchorfile>
      <anchor>aebd30f5cebe1ca27cfdfd8354a0fa805</anchor>
      <arglist>(OCPDim, OCPDim)=default</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::parallel::Context</name>
    <filename>structcyqlone_1_1parallel_1_1Context.html</filename>
    <templarg>class SC</templarg>
    <member kind="typedef">
      <type>SC</type>
      <name>shared_context_type</name>
      <anchorfile>structcyqlone_1_1parallel_1_1Context.html</anchorfile>
      <anchor>aa99484189beec640c46e1fb396493f67</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename shared_context_type::barrier_type::arrival_token</type>
      <name>arrival_token</name>
      <anchorfile>structcyqlone_1_1parallel_1_1Context.html</anchorfile>
      <anchor>a89686f17bc2ad6f01c5f6a96d7042048</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>is_master</name>
      <anchorfile>structcyqlone_1_1parallel_1_1Context.html</anchorfile>
      <anchor>a887da19d804556462d028e09f557124d</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>arrival_token</type>
      <name>arrive</name>
      <anchorfile>structcyqlone_1_1parallel_1_1Context.html</anchorfile>
      <anchor>aef10e2b4232a902976b61df51b076479</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>wait</name>
      <anchorfile>structcyqlone_1_1parallel_1_1Context.html</anchorfile>
      <anchor>ae266e8ec4d5ef7380b5d882fbe641e70</anchor>
      <arglist>(arrival_token &amp;&amp;token)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>arrive_and_wait</name>
      <anchorfile>structcyqlone_1_1parallel_1_1Context.html</anchorfile>
      <anchor>a6963c65991275ff3585e3e688bcb551c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>arrive_and_wait</name>
      <anchorfile>structcyqlone_1_1parallel_1_1Context.html</anchorfile>
      <anchor>acc28682a90042ba9237897afb57e9480</anchor>
      <arglist>(int line)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>broadcast</name>
      <anchorfile>structcyqlone_1_1parallel_1_1Context.html</anchorfile>
      <anchor>a0f46566118026dbf50b3fa78d05aaa19</anchor>
      <arglist>(T x, index_t src=0)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>call_broadcast</name>
      <anchorfile>structcyqlone_1_1parallel_1_1Context.html</anchorfile>
      <anchor>a3738b26f863e058b5f67bf605d578673</anchor>
      <arglist>(F &amp;&amp;f, Args &amp;&amp;...args) -&gt; std::invoke_result_t&lt; F, Args... &gt;</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>arrive_reduce</name>
      <anchorfile>structcyqlone_1_1parallel_1_1Context.html</anchorfile>
      <anchor>aa993898c2ab5cc872e6b012fde96f064</anchor>
      <arglist>(T x, F func)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>wait_reduce</name>
      <anchorfile>structcyqlone_1_1parallel_1_1Context.html</anchorfile>
      <anchor>aa9b103c071e149666500fc45589417b0</anchor>
      <arglist>(shared_context_type::barrier_type::template arrival_token_typed&lt; T &gt; &amp;&amp;token)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>reduce</name>
      <anchorfile>structcyqlone_1_1parallel_1_1Context.html</anchorfile>
      <anchor>acfda285f17d9c76cb07d0a62c4ca42d2</anchor>
      <arglist>(T x, F func)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>reduce</name>
      <anchorfile>structcyqlone_1_1parallel_1_1Context.html</anchorfile>
      <anchor>a35e4cdfc4b57d6c60e8edc126dbc4a83</anchor>
      <arglist>(T x)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>run_single_sync</name>
      <anchorfile>structcyqlone_1_1parallel_1_1Context.html</anchorfile>
      <anchor>a42fd8615d3d4fa66a36319cc5aef3b91</anchor>
      <arglist>(F &amp;&amp;f)</arglist>
    </member>
    <member kind="variable">
      <type>shared_context_type &amp;</type>
      <name>shared</name>
      <anchorfile>structcyqlone_1_1parallel_1_1Context.html</anchorfile>
      <anchor>af571ba6868ab56af0d2a510891d48ac3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const index_t</type>
      <name>index</name>
      <anchorfile>structcyqlone_1_1parallel_1_1Context.html</anchorfile>
      <anchor>ada96a787c05abfa1ec56308fa189d7a6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const index_t</type>
      <name>num_thr</name>
      <anchorfile>structcyqlone_1_1parallel_1_1Context.html</anchorfile>
      <anchor>af82ec77bbfa2eb48834ed8971c51b45e</anchor>
      <arglist></arglist>
    </member>
    <member kind="friend">
      <type>friend constexpr bool</type>
      <name>operator==</name>
      <anchorfile>structcyqlone_1_1parallel_1_1Context.html</anchorfile>
      <anchor>ae464c8a4dc0d8acc3e410fa51d3c33fd</anchor>
      <arglist>(const Context &amp;a, const Context &amp;b)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::parallel::SharedContext</name>
    <filename>structcyqlone_1_1parallel_1_1SharedContext.html</filename>
    <member kind="typedef">
      <type>EmptyCompletion</type>
      <name>completion_type</name>
      <anchorfile>structcyqlone_1_1parallel_1_1SharedContext.html</anchorfile>
      <anchor>a8541d7bb84b40a889200aa2b8281e455</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>TreeBarrier&lt; completion_type, uint16_t &gt;</type>
      <name>barrier_type</name>
      <anchorfile>structcyqlone_1_1parallel_1_1SharedContext.html</anchorfile>
      <anchor>a8c23779f4d3e9282e8a0f4414a7a02df</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>run</name>
      <anchorfile>structcyqlone_1_1parallel_1_1SharedContext.html</anchorfile>
      <anchor>a070d23cba7fbc854c1834e130eaaa1ec</anchor>
      <arglist>(F &amp;&amp;)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>set_barrier_spin_count</name>
      <anchorfile>structcyqlone_1_1parallel_1_1SharedContext.html</anchorfile>
      <anchor>a2d25e5fdf88c13360ada15986fcc98d1</anchor>
      <arglist>(uint32_t spin_count)</arglist>
    </member>
    <member kind="variable">
      <type>const index_t</type>
      <name>num_thr</name>
      <anchorfile>structcyqlone_1_1parallel_1_1SharedContext.html</anchorfile>
      <anchor>a1b0dfe21418e9ebf681373cea1b1dd97</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>barrier_type</type>
      <name>barrier</name>
      <anchorfile>structcyqlone_1_1parallel_1_1SharedContext.html</anchorfile>
      <anchor>a036e911c7fd2098f291a7cc59e395765</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::PCRFactorTest</name>
    <filename>structcyqlone_1_1PCRFactorTest.html</filename>
    <templarg>index_t VL</templarg>
    <templarg>class T</templarg>
    <templarg>StorageOrder DefaultOrder</templarg>
    <member kind="typedef">
      <type>T</type>
      <name>value_type</name>
      <anchorfile>structcyqlone_1_1PCRFactorTest.html</anchorfile>
      <anchor>aca9db3f60b5726ab772b6dcf1ebebb7c</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>std::integral_constant&lt; index_t, VL &gt;</type>
      <name>vl_t</name>
      <anchorfile>structcyqlone_1_1PCRFactorTest.html</anchorfile>
      <anchor>a009916850f2956dfb3cc4dc3095ac96d</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>std::integral_constant&lt; index_t, VL *alignof(T)&gt;</type>
      <name>align_t</name>
      <anchorfile>structcyqlone_1_1PCRFactorTest.html</anchorfile>
      <anchor>a2e383cb7f58ffcf0a3a4c28e05ebe6f6</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>batmat::matrix::Matrix&lt; value_type, index_t, vl_t, index_t, O, align_t &gt;</type>
      <name>bmatrix</name>
      <anchorfile>structcyqlone_1_1PCRFactorTest.html</anchorfile>
      <anchor>a0796a706ee7eefe3903e0af5b7a6ac24</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>batmat::matrix::Matrix&lt; value_type, index_t, vl_t, vl_t, O, align_t &gt;</type>
      <name>matrix</name>
      <anchorfile>structcyqlone_1_1PCRFactorTest.html</anchorfile>
      <anchor>a4dab56b41d82c2b9a8d73b71b2717442</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>matrix&lt; O &gt;::view_type</type>
      <name>mut_view</name>
      <anchorfile>structcyqlone_1_1PCRFactorTest.html</anchorfile>
      <anchor>a4d28f52645afaeba06b8db248f9d8618</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>matrix&lt; O &gt;::const_view_type</type>
      <name>view</name>
      <anchorfile>structcyqlone_1_1PCRFactorTest.html</anchorfile>
      <anchor>a23b6f97fffaae2f2f64c34b85cf8ea58</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>factor_pcr</name>
      <anchorfile>structcyqlone_1_1PCRFactorTest.html</anchorfile>
      <anchor>a36949d781145d957b57d53208f74d9fa</anchor>
      <arglist>(view&lt;&gt; M0, view&lt;&gt; K0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>factor_pcr_level</name>
      <anchorfile>structcyqlone_1_1PCRFactorTest.html</anchorfile>
      <anchor>a2615315b158b8a26e8e85cf070120575</anchor>
      <arglist>(view&lt;&gt; M0, view&lt;&gt; K0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve_pcr</name>
      <anchorfile>structcyqlone_1_1PCRFactorTest.html</anchorfile>
      <anchor>a4c5a042d9bc82a9dd62e57a171db16c3</anchor>
      <arglist>(mut_view&lt;&gt; λ)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve_pcr</name>
      <anchorfile>structcyqlone_1_1PCRFactorTest.html</anchorfile>
      <anchor>afbaa848d6b95cefd877e9fcbba209c16</anchor>
      <arglist>(mut_view&lt;&gt; λ, mut_view&lt;&gt; work_pcr) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve_pcr_level</name>
      <anchorfile>structcyqlone_1_1PCRFactorTest.html</anchorfile>
      <anchor>a8dfeed2551482c24d36fed815072cbe5</anchor>
      <arglist>(mut_view&lt;&gt; λ, mut_view&lt;&gt; work_pcr) const</arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>n</name>
      <anchorfile>structcyqlone_1_1PCRFactorTest.html</anchorfile>
      <anchor>a1744ddc978aa75c1c78d996cb8365ebc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bmatrix</type>
      <name>pcr_L</name>
      <anchorfile>structcyqlone_1_1PCRFactorTest.html</anchorfile>
      <anchor>a09dd56d6e4bc3fecfae0799f87232a4b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bmatrix</type>
      <name>pcr_Y</name>
      <anchorfile>structcyqlone_1_1PCRFactorTest.html</anchorfile>
      <anchor>ab048dce464f3f844fd5f3281c5cda611</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bmatrix</type>
      <name>pcr_U</name>
      <anchorfile>structcyqlone_1_1PCRFactorTest.html</anchorfile>
      <anchor>ae240c0792cdd5a4e39afd38b7a3e0974</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix</type>
      <name>pcr_M</name>
      <anchorfile>structcyqlone_1_1PCRFactorTest.html</anchorfile>
      <anchor>a899d7b61892ca3df5bcb7f8d66016552</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix</type>
      <name>work</name>
      <anchorfile>structcyqlone_1_1PCRFactorTest.html</anchorfile>
      <anchor>ab826c73f29eb3b50108c82003c4be946</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static constexpr index_t</type>
      <name>v</name>
      <anchorfile>structcyqlone_1_1PCRFactorTest.html</anchorfile>
      <anchor>ace6465f995fe5f1d403e1fe795f3ea4c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static constexpr index_t</type>
      <name>lv</name>
      <anchorfile>structcyqlone_1_1PCRFactorTest.html</anchorfile>
      <anchor>ace34111c93973ae12caae65d094c17b2</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::ABSums</name>
    <filename>structcyqlone_1_1qpalm_1_1ABSums.html</filename>
    <member kind="variable">
      <type>ABSum_t</type>
      <name>a</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1ABSums.html</anchorfile>
      <anchor>a8670a763f4b93f9828cd332cfec090a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>ABSum_t</type>
      <name>b</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1ABSums.html</anchorfile>
      <anchor>a105db14f929ad2468f1b41ebfb09b92f</anchor>
      <arglist></arglist>
    </member>
    <member kind="friend">
      <type>constexpr friend ABSums</type>
      <name>operator+</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1ABSums.html</anchorfile>
      <anchor>ad64dcba97952dece7508e4b6935335ce</anchor>
      <arglist>(const ABSums &amp;lhs, const ABSums &amp;rhs)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::Breakpoint</name>
    <filename>structcyqlone_1_1qpalm_1_1Breakpoint.html</filename>
    <member kind="function">
      <type>real_t</type>
      <name>α</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Breakpoint.html</anchorfile>
      <anchor>a3b71b2f757fe16a5631fb31ff9efe565</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>t</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Breakpoint.html</anchorfile>
      <anchor>a094b11f513e341a4a772048859576503</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>δ</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Breakpoint.html</anchorfile>
      <anchor>a5323f4decf9616946291234021820120</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::BreakpointsResult</name>
    <filename>namespacecyqlone_1_1qpalm.html</filename>
    <anchor>structcyqlone_1_1qpalm_1_1BreakpointsResult</anchor>
    <member kind="variable">
      <type>PartitionedBreakpoints</type>
      <name>bp</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>abe3f31fef0fe9ec9df164a59a621c1ce</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>ABSums</type>
      <name>ab_neg</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>ac56a2f8be122302fb793476a68f1e0f0</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::compute_breakpoints_fn</name>
    <filename>structcyqlone_1_1qpalm_1_1compute__breakpoints__fn.html</filename>
    <member kind="typedef">
      <type>typename std::remove_cvref_t&lt; Backend &gt;::ineq_constr_vec_t</type>
      <name>vec_t</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1compute__breakpoints__fn.html</anchorfile>
      <anchor>a298d4f4bdb51013f64c72457c2430c2e</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>operator()</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1compute__breakpoints__fn.html</anchorfile>
      <anchor>a87a41f81975359188235249804f938a7</anchor>
      <arglist>(Backend &amp;backend, typename Backend::Context &amp;ctx, std::vector&lt; Breakpoint &gt; &amp;breakpoints, const vec_t&lt; Backend &gt; &amp;Σ, const vec_t&lt; Backend &gt; &amp;y, const vec_t&lt; Backend &gt; &amp;Ad, const vec_t&lt; Backend &gt; &amp;Ax, const vec_t&lt; Backend &gt; &amp;b_min, const vec_t&lt; Backend &gt; &amp;b_max) const noexcept(guanaqo::is_nothrow_tag_invocable_v&lt; compute_breakpoints_fn, Backend &amp;, typename Backend::Context &amp;, std::vector&lt; Breakpoint &gt; &amp;, const vec_t&lt; Backend &gt; &amp;, const vec_t&lt; Backend &gt; &amp;, const vec_t&lt; Backend &gt; &amp;, const vec_t&lt; Backend &gt; &amp;, const vec_t&lt; Backend &gt; &amp;, const vec_t&lt; Backend &gt; &amp; &gt;)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>operator()</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1compute__breakpoints__fn.html</anchorfile>
      <anchor>ae2965682e3646b02b83388fcbb3233b7</anchor>
      <arglist>(Backend &amp;, typename Backend::Context &amp;ctx, std::vector&lt; Breakpoint &gt; &amp;breakpoints, const vec_t&lt; Backend &gt; &amp;Σ, const vec_t&lt; Backend &gt; &amp;y, const vec_t&lt; Backend &gt; &amp;Ad, const vec_t&lt; Backend &gt; &amp;Ax, const vec_t&lt; Backend &gt; &amp;b_min, const vec_t&lt; Backend &gt; &amp;b_max) const</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::CyqloneData</name>
    <filename>group__topic-optimization-solvers-ocp.html</filename>
    <anchor>structcyqlone_1_1qpalm_1_1CyqloneData</anchor>
    <member kind="variable">
      <type>std::span&lt; const real_t &gt;</type>
      <name>initial_variables</name>
      <anchorfile>group__topic-optimization-solvers-ocp.html</anchorfile>
      <anchor>aa9df25a26d4bf890fda706b310db2faf</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::span&lt; const real_t &gt;</type>
      <name>initial_inequality_multipliers</name>
      <anchorfile>group__topic-optimization-solvers-ocp.html</anchorfile>
      <anchor>a8fcb8221fd005fd9e44c5593cb33855d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::span&lt; const real_t &gt;</type>
      <name>initial_equality_multipliers</name>
      <anchorfile>group__topic-optimization-solvers-ocp.html</anchorfile>
      <anchor>a518733daa3ca0eb03f0d048a1756ca1e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::CyQPALMBackend</name>
    <filename>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</filename>
    <templarg>index_t VL</templarg>
    <templarg>StorageOrder DefaultOrder</templarg>
    <member kind="function">
      <type>BreakpointsResult</type>
      <name>compute_partition_breakpoints</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a640bb174c3f290758b4336f4304e1512</anchor>
      <arglist>(Context &amp;ctx, std::vector&lt; Breakpoint &gt; &amp;breakpoints, const ineq_constr_vec_t &amp;Σ, const ineq_constr_vec_t &amp;y, const ineq_constr_vec_t &amp;Ad, const ineq_constr_vec_t &amp;Ax, const ineq_constr_vec_t &amp;b_min, const ineq_constr_vec_t &amp;b_max)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>merge_chunk</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>aa16e6468de8c3ff8f2ad48db39b51a74</anchor>
      <arglist>(std::span&lt; const T &gt; chunk, size_t chunk_index, std::span&lt; const std::array&lt; size_t, N &gt; &gt; separators, std::span&lt; T &gt; out)</arglist>
    </member>
    <member kind="friend">
      <type>friend BreakpointsResult</type>
      <name>guanaqo_tag_invoke</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>ab7ed283c88ad466c65ddbe181abbf3a8</anchor>
      <arglist>(guanaqo::tag_t&lt; get_breakpoints &gt;, CyQPALMBackend &amp;backend, Context &amp;ctx, std::vector&lt; Breakpoint &gt; &amp;breakpoints, const ineq_constr_vec_t &amp;Σ, const ineq_constr_vec_t &amp;y, const ineq_constr_vec_t &amp;Ad, const ineq_constr_vec_t &amp;Ax, const ineq_constr_vec_t &amp;b_min, const ineq_constr_vec_t &amp;b_max)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>xaxpy</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a79c7e19ce1b2fc5036bc7b47f94f1773</anchor>
      <arglist>(Context &amp;ctx, real_t a, const T &amp;x, U &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>xcopy</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a131cdd524696e77251482785264ca61d</anchor>
      <arglist>(Context &amp;ctx, const T &amp;x, U &amp;y) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_constant</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>af682fe6bb7af7bb3da30dc6a41da8e9e</anchor>
      <arglist>(Context &amp;ctx, T &amp;x, const U &amp;y) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scale</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>af9270655befe5a4a56f6fceb855209f0</anchor>
      <arglist>(Context &amp;ctx, real_t s, T &amp;x) const</arglist>
    </member>
    <member kind="function">
      <type>real_t</type>
      <name>dot</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a148581e9d54fed42c92ac7db4c257e5a</anchor>
      <arglist>(Context &amp;ctx, const var_vec_t &amp;a, const var_vec_t &amp;b) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>local_dots</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a0d0ecf5274d4786340d8d2a4e2fd29b2</anchor>
      <arglist>(std::span&lt; real_t, 1+sizeof...(Args)/2 &gt; out, const auto &amp;a, const auto &amp;b, const Args &amp;...others) const</arglist>
    </member>
    <member kind="function">
      <type>std::array&lt; real_t, sizeof...(Args)/2 &gt;</type>
      <name>dots</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>afffa753eaac33504cee0e1ffea679522</anchor>
      <arglist>(Context &amp;ctx, const Args &amp;...args) const</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>norm_inf_l1_sq</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>ad5c970294e1768863a6d6a0880b032b2</anchor>
      <arglist>(Context &amp;ctx, const T &amp;x) const</arglist>
    </member>
    <member kind="function">
      <type>real_t</type>
      <name>norm_inf</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a23f573e7caa20f2bff4783c01694b092</anchor>
      <arglist>(Context &amp;ctx, const T &amp;x) const</arglist>
    </member>
    <member kind="function">
      <type>real_t</type>
      <name>norm_squared</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>acd9901cf7ae42604bd0ccea0861ba39d</anchor>
      <arglist>(Context &amp;ctx, const T &amp;x) const</arglist>
    </member>
    <class kind="struct">cyqlone::qpalm::CyQPALMBackend::var_vec_t</class>
    <class kind="struct">cyqlone::qpalm::CyQPALMBackend::eq_constr_vec_t</class>
    <class kind="struct">cyqlone::qpalm::CyQPALMBackend::ineq_constr_vec_t</class>
    <class kind="struct">cyqlone::qpalm::CyQPALMBackend::active_set_t</class>
    <member kind="typedef">
      <type>cyqlone::CyqloneSolver&lt; VL, real_t, DefaultOrder &gt;</type>
      <name>OCP_t</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a34013b2ade46dadd81911b3deebc8454</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename OCP_t::Context</type>
      <name>Context</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a4ff7a222bb1d9378d9f7502f99689a5b</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename OCP_t::template matrix&lt;&gt;</type>
      <name>storage_t</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>ad70fc5596db9b5cf41fd4b66a27a414f</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename OCP_t::simd</type>
      <name>simd</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>ace517694555931d5b223321bd7f23961</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>CyQPALMBackendStats</type>
      <name>Stats</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a441001ae1e41803448dcf0b1db4d0bf7</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CyQPALMBackend</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a0216d970f15950673149cc27275df469</anchor>
      <arglist>(const CyqloneStorage&lt;&gt; &amp;ocp, CyqloneData data, const CyQPALMBackendSettings &amp;settings)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update_data</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a422f861e115493145d188b75c69b9fc5</anchor>
      <arglist>(const CyqloneStorage&lt;&gt; &amp;ocp)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_b_eq</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a0b650bda524edc454cc9247884f9438e</anchor>
      <arglist>(std::span&lt; const real_t &gt; b_eq)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_b_lb</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a7b57d053d9e5376a736e63e64aea1ca6</anchor>
      <arglist>(std::span&lt; const real_t &gt; b_lb)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_b_ub</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a4cd3aad46fb6bd3a53203edcbca17dc4</anchor>
      <arglist>(std::span&lt; const real_t &gt; b_ub)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>warm_start</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a63107e32e493e60a87e7e98e52a5a958</anchor>
      <arglist>(const var_vec_t &amp;x, const ineq_constr_vec_t &amp;y, const eq_constr_vec_t &amp;λ)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a26e471ea5e8bcd282f960ee74822d187</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>num_var</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>ab328a92790edb1a530d56b03b26e0d80</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>num_eq_constr</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a1ec18687a7be5b45aac8dbab0f4a4af4</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>num_ineq_constr</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>afd84e929b5a413a73b9535b999c2a52c</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>var_vec_t</type>
      <name>var_vec</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a717be44f79c8a4739fa22408ecd0d274</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>eq_constr_vec_t</type>
      <name>eq_constr_vec</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a4dd3856f338be34236fb9da5c53deb08</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>ineq_constr_vec_t</type>
      <name>ineq_constr_vec</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>aa1252528521f02a5bbb7555d4ba162d4</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>active_set_t</type>
      <name>active_set</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a6c3ad5067413aba2d0fe552fe246897e</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>initialize_active_set</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a0254f171f4e360107a67eacd2f0e4d39</anchor>
      <arglist>(Js &amp;...js) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>initialize_var_vec</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>af112a90fe7be7edaee2bf9998b341044</anchor>
      <arglist>(Xs &amp;...xs) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>initialize_ineq_constr_vec</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>ab8888ef51a817e47fa466b5886aa8d2a</anchor>
      <arglist>(Ys &amp;...ys) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>initialize_eq_constr_vec</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>aaea6aa50f60e409f2d488e54f4b9f14d</anchor>
      <arglist>(Λs &amp;...λs) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>initial_variables</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a0a42300dfab85c6eec0304126657bfc6</anchor>
      <arglist>(Context &amp;ctx, var_vec_t &amp;x) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>initial_multipliers_eq</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a5b05155c046c07d747f61ec5fa3398ff</anchor>
      <arglist>(Context &amp;ctx, eq_constr_vec_t &amp;λ) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>initial_multipliers_ineq</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a175960717d260a30aee603ee985be1bf</anchor>
      <arglist>(Context &amp;ctx, ineq_constr_vec_t &amp;y) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>ineq_constr_resid</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>ad8eefaeda3b0c0431edb53347381b41d</anchor>
      <arglist>(Context &amp;ctx, const ineq_constr_vec_t &amp;Ax, ineq_constr_vec_t &amp;e) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>project_multipliers_ineq</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a6d475b2639810c3a7cc5a59ed7fcf310</anchor>
      <arglist>(Context &amp;ctx, ineq_constr_vec_t &amp;y) const</arglist>
    </member>
    <member kind="function">
      <type>real_t</type>
      <name>ineq_constr_viol</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a1d8193326be5026e3a333880a75c6d63</anchor>
      <arglist>(Context &amp;ctx, const ineq_constr_vec_t &amp;Ax) const</arglist>
    </member>
    <member kind="function">
      <type>real_t</type>
      <name>ineq_constr_resid_al</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>acb9d537c6c1b9358b962b4c0a4fef284</anchor>
      <arglist>(Context &amp;ctx, const ineq_constr_vec_t &amp;y, const ineq_constr_vec_t &amp;ŷ, const ineq_constr_vec_t &amp;Σ, ineq_constr_vec_t &amp;e)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>eq_constr_resid</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a96977da98f2541831d8b7cd483526d76</anchor>
      <arglist>(Context &amp;ctx, const var_vec_t &amp;x, eq_constr_vec_t &amp;Mxb)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>mat_vec_MT</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a817a804c0fd29ce5a8d8f3ce45512a70</anchor>
      <arglist>(Context &amp;ctx, const eq_constr_vec_t &amp;λ, var_vec_t &amp;Mᵀλ)</arglist>
    </member>
    <member kind="function">
      <type>real_t</type>
      <name>unscaled_eq_constr_viol</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a3867d806cbfa4a82b4158991a55cd2d3</anchor>
      <arglist>(Context &amp;ctx, const eq_constr_vec_t &amp;Mxb) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>mat_vec_AT</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>afdb4e99a2009b24b7d5777ab25a874cd</anchor>
      <arglist>(Context &amp;ctx, const ineq_constr_vec_t &amp;y, var_vec_t &amp;Aᵀy)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>mat_vec_AT</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>aab6dbeec8089e01390d6ebc58e1c5715</anchor>
      <arglist>(const ineq_constr_vec_t &amp;y, var_vec_t &amp;Aᵀy)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>mat_vec_A</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>acb936703272de81656fda980af871412</anchor>
      <arglist>(Context &amp;ctx, const var_vec_t &amp;x, ineq_constr_vec_t &amp;Ax)</arglist>
    </member>
    <member kind="function">
      <type>ineq_constr_vec_t</type>
      <name>mat_vec_A</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>adcc672248f0d54175c5a03778ed53458</anchor>
      <arglist>(Context &amp;ctx, const var_vec_t &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>grad_f</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>ab9e9036ffce88fb29b629822a61fba84</anchor>
      <arglist>(Context &amp;ctx, const var_vec_t &amp;x, var_vec_t &amp;grad_f)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>grad_f_regularized</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>ad4addb66dee94577abf4c74feceaa544</anchor>
      <arglist>(Context &amp;ctx, real_t S, const var_vec_t &amp;x, const var_vec_t &amp;x_reg, var_vec_t &amp;grad_f)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>grad_f_remove_regularization</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a3df5c7f2f582ac25f10c1cccf194d2ad</anchor>
      <arglist>(Context &amp;ctx, real_t S, const var_vec_t &amp;x, const var_vec_t &amp;x_reg, var_vec_t &amp;grad_f)</arglist>
    </member>
    <member kind="function">
      <type>real_t</type>
      <name>f_grad_f</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>afe18bcfe2d05a8b5386e6759b5bc5049</anchor>
      <arglist>(Context &amp;ctx, const var_vec_t &amp;x, var_vec_t &amp;grad_f)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; real_t, var_vec_t &gt;</type>
      <name>f_grad_f</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a467036ca9a5f9143a93d4c01abd5f070</anchor>
      <arglist>(Context &amp;ctx, const var_vec_t &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>update_penalty_y</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a87002bf004a143c776df2c46e6d74d75</anchor>
      <arglist>(Context &amp;ctx, ineq_constr_vec_t &amp;Σ, const ineq_constr_vec_t &amp;e, const ineq_constr_vec_t &amp;e_old, const PenaltySettings &amp;settings)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update_regularization_changed</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a274accbcf625fdb0ba5f2402be684d78</anchor>
      <arglist>(Context &amp;ctx, real_t S_new, real_t S_old)</arglist>
    </member>
    <member kind="function">
      <type>real_t</type>
      <name>boost_regularization</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a5328676d6cbea5c5fafea68aa72a330e</anchor>
      <arglist>(Context &amp;ctx, real_t S, real_t S_boost)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update_penalty_changed</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a99e4e5c0aec1b944d298dc7ac2ad77c0</anchor>
      <arglist>(Context &amp;ctx, const ineq_constr_vec_t &amp;Σ, index_t num_Σ_changed)</arglist>
    </member>
    <member kind="function">
      <type>const ineq_constr_vec_t &amp;</type>
      <name>Ax_min</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a3904383a1138f9e7b91b3c9ab9fd665c</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>const ineq_constr_vec_t &amp;</type>
      <name>Ax_max</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a4f6515b8301b52bf1e889d8c39a2dfe4</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>calc_ŷ_Aᵀŷ</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a1e7c2a87b109b39d2d38a59feea88109</anchor>
      <arglist>(Context &amp;ctx, const ineq_constr_vec_t &amp;Ax, const ineq_constr_vec_t &amp;Σ, const ineq_constr_vec_t &amp;y, ineq_constr_vec_t &amp;ŷ, var_vec_t &amp;Aᵀŷ, active_set_t &amp;J)</arglist>
    </member>
    <member kind="function">
      <type>real_t</type>
      <name>unscaled_aug_lagr_norm</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>ae88bfe00d27548310cec8f40e76abce0</anchor>
      <arglist>(Context &amp;ctx, const var_vec_t &amp;grad_f, const var_vec_t &amp;Mᵀλ, const var_vec_t &amp;Aᵀŷ) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scale_variables</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>aa80da218cd325640499d0edc0e8f8592</anchor>
      <arglist>(std::span&lt; const real_t &gt; in, var_vec_t &amp;out) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scale_ineq_constr</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a8702120039a23bd7466a514ad829196f</anchor>
      <arglist>(std::span&lt; const real_t &gt; in, ineq_constr_vec_t &amp;out) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scale_eq_constr</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>ae545c885936cbe284b749c0044bbe125</anchor>
      <arglist>(std::span&lt; const real_t &gt; in, eq_constr_vec_t &amp;out) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>unscale_variables</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a9fa68b8a71248e2013eb3ba8e235139b</anchor>
      <arglist>(const var_vec_t &amp;in, std::span&lt; real_t &gt; out) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>unscale_ineq_constr</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a5f214f7a2ce0fbd662824fb6f067c892</anchor>
      <arglist>(const ineq_constr_vec_t &amp;in, std::span&lt; real_t &gt; out) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>unscale_ineq_constr</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a49bccad0883835e660e95b520e77c14d</anchor>
      <arglist>(const active_set_t &amp;in, std::span&lt; real_t &gt; out) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>unscale_eq_constr</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>ae875f80e6468b9241077f6ea1d763a33</anchor>
      <arglist>(const eq_constr_vec_t &amp;in, std::span&lt; real_t &gt; out) const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>active_set_change</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a5df9dcb8311b8e5f96bf5faa2d512719</anchor>
      <arglist>(Context &amp;ctx, real_t, const ineq_constr_vec_t &amp;Σ, const active_set_t &amp;J, const active_set_t &amp;J_old)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>recompute_inner</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a061e0f142c42b5d959d57625395238e9</anchor>
      <arglist>(Context &amp;ctx, real_t S, const var_vec_t &amp;x_outer, const var_vec_t &amp;x, const eq_constr_vec_t &amp;λ, var_vec_t &amp;grad, ineq_constr_vec_t &amp;Ax, var_vec_t &amp;Mᵀλ)</arglist>
    </member>
    <member kind="function">
      <type>real_t</type>
      <name>recompute_outer</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a20f212a3184f94905e9f12df35611ced</anchor>
      <arglist>(Context &amp;ctx, const var_vec_t &amp;x, const var_vec_t &amp;Aᵀŷ, const eq_constr_vec_t &amp;λ, var_vec_t &amp;grad, ineq_constr_vec_t &amp;Ax, var_vec_t &amp;Mᵀλ)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print_solve_rhs_norms</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a8d445e40b3a8a0073c6f90dafb9e3323</anchor>
      <arglist>(Context &amp;ctx, const var_vec_t &amp;d, const eq_constr_vec_t &amp;Δλ, const var_vec_t &amp;grad, const var_vec_t &amp;Mᵀλ, const var_vec_t &amp;Aᵀŷ) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print_solve_resid_norms</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>aa60ef9263ef269cd44f2dce874fae6d0</anchor>
      <arglist>(Context &amp;ctx, const var_vec_t &amp;x, const var_vec_t &amp;d, const var_vec_t &amp;grad, const var_vec_t &amp;ξ, const var_vec_t &amp;Mᵀλ, const var_vec_t &amp;Aᵀŷ, const var_vec_t &amp;MᵀΔλ, const ineq_constr_vec_t &amp;Ad, const active_set_t &amp;J)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>abee4ac957116405888f686e110e719db</anchor>
      <arglist>(Context &amp;ctx, const var_vec_t &amp;x, const var_vec_t &amp;grad, const var_vec_t &amp;Mᵀλ, const var_vec_t &amp;Aᵀŷ, const eq_constr_vec_t &amp;Mxb, real_t S, const ineq_constr_vec_t &amp;Σ, const active_set_t &amp;J, var_vec_t &amp;d, var_vec_t &amp;ξ, ineq_constr_vec_t &amp;Ad, eq_constr_vec_t &amp;Δλ, var_vec_t &amp;MᵀΔλ)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>get_timed</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a3a772f16b1a0ef34e91c8ad488f565a2</anchor>
      <arglist>(Timings::type Timings::*member) const</arglist>
    </member>
    <member kind="function">
      <type>Stats</type>
      <name>clear_stats</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a645c8c5a71b9da8c641e4d766e02db5c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::map&lt; std::string, typename Timings::type &gt;</type>
      <name>clear_timings</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a787d403b609e78ec53f1d9a00d626a19</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable">
      <type>OCP_t</type>
      <name>ocp</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a9136b75d6dc0873e9b596a1a6e3a2f6e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::unique_ptr&lt; typename OCP_t::SharedContext &gt;</type>
      <name>parallel_ctx</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>af282fc5d1ddf6c7f5c3a7b615367c1ea</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>CyQPALMBackendSettings</type>
      <name>settings</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a5aee9a69e5a69972ae922970a6598625</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>ineq_constr_vec_t</type>
      <name>b_min_strided</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>ace528e2b25eea563fe60068f3eebee98</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>ineq_constr_vec_t</type>
      <name>b_max_strided</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a44ad7dfbff49abe09d7d33232bf86638</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>eq_constr_vec_t</type>
      <name>b_eq_strided</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>af56b21463035e447565d2254a6b4a2d2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>var_vec_t</type>
      <name>grad_strided</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a35f07258cb24211ce2bec0772081ca27</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>ineq_constr_vec_t</type>
      <name>ΔΣ</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a3dd8af8b5013cd2c14a1658e055cbac9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::optional&lt; var_vec_t &gt;</type>
      <name>x0</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>ad21cf2e5a057355cd1e89092defd44b4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::optional&lt; ineq_constr_vec_t &gt;</type>
      <name>y0</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>acdb27a3e00ab262703a6f77bbf36efe3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::optional&lt; eq_constr_vec_t &gt;</type>
      <name>λ0</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>ae239deecfcbbd207a6ef4f9e25d7023d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; std::array&lt; size_t, 4 &gt; &gt;</type>
      <name>thread_indices</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a4055cf1505b5b03ab656974cff706982</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; Breakpoint &gt;</type>
      <name>breakpoints_temp</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>adea6a5ba64814771940995594daf0558</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>reset_factorization</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a591c73536f98a4fdf35e0b5338a4a8a9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>update_pending</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>adfd301007cbd74cba17d7d1c4d739aa1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>num_updates</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a50364ed71ef1ae9586f51c7938e54d7f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::unique_ptr&lt; Timings &gt;</type>
      <name>ocp_timings</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>aff28fda89aaed2fa5f56ad544a5c6283</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>var_vec_t</type>
      <name>temp_var</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a8e1cd0dfe7b42bb32bd3083d3862a2cd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>eq_constr_vec_t</type>
      <name>temp_eq</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a08ac67250d3d71adc8596700b2787296</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>ineq_constr_vec_t</type>
      <name>temp_ineq</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a334bc2d64949f3230cd8d7e0972b64a9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Stats</type>
      <name>stats</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>aa125f56a30f0d8b0476773dbfdef4f53</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static constexpr auto</type>
      <name>norms</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>afe8b0bb8603a11c5f3f1d231fde5a1e4</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::CyQPALMBackend::active_set_t</name>
    <filename>structcyqlone_1_1qpalm_1_1CyQPALMBackend_1_1active__set__t.html</filename>
    <member kind="function">
      <type></type>
      <name>active_set_t</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend_1_1active__set__t.html</anchorfile>
      <anchor>ab79d4f5bbeb5638ba59218b9e8885921</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="variable">
      <type>friend</type>
      <name>CyQPALMBackend</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend_1_1active__set__t.html</anchorfile>
      <anchor>a300eb9b4cb5df0e67fa932395906b64e</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" protection="private">
      <type></type>
      <name>active_set_t</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend_1_1active__set__t.html</anchorfile>
      <anchor>ae3f5385d3fb7419a1aec30a041067675</anchor>
      <arglist>(storage_t &amp;&amp;o)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::CyQPALMBackend::eq_constr_vec_t</name>
    <filename>structcyqlone_1_1qpalm_1_1CyQPALMBackend_1_1eq__constr__vec__t.html</filename>
    <member kind="function">
      <type></type>
      <name>eq_constr_vec_t</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend_1_1eq__constr__vec__t.html</anchorfile>
      <anchor>a509833db7dcd1d3d55a1940fdd23f1c2</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="variable">
      <type>friend</type>
      <name>CyQPALMBackend</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend_1_1eq__constr__vec__t.html</anchorfile>
      <anchor>a1696e320a740ee727082b1e292df4e8c</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" protection="private">
      <type></type>
      <name>eq_constr_vec_t</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend_1_1eq__constr__vec__t.html</anchorfile>
      <anchor>a6e2a9aadab7d45746efa5ac8424b19b7</anchor>
      <arglist>(storage_t &amp;&amp;o)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::CyQPALMBackend::ineq_constr_vec_t</name>
    <filename>structcyqlone_1_1qpalm_1_1CyQPALMBackend_1_1ineq__constr__vec__t.html</filename>
    <member kind="function">
      <type></type>
      <name>ineq_constr_vec_t</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend_1_1ineq__constr__vec__t.html</anchorfile>
      <anchor>a1584afbe7e66715a959ee2298968469a</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="variable">
      <type>friend</type>
      <name>CyQPALMBackend</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend_1_1ineq__constr__vec__t.html</anchorfile>
      <anchor>a7ceda44eb3b269c4e6cd7413ab08c31c</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" protection="private">
      <type></type>
      <name>ineq_constr_vec_t</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend_1_1ineq__constr__vec__t.html</anchorfile>
      <anchor>a2e164e06da7bd8303af1229d8b6d85e3</anchor>
      <arglist>(storage_t &amp;&amp;o)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::CyQPALMBackend::PenaltySettings</name>
    <filename>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</filename>
    <anchor>structcyqlone_1_1qpalm_1_1CyQPALMBackend_1_1PenaltySettings</anchor>
    <member kind="variable">
      <type>real_t</type>
      <name>θ</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>ab415575db2fb46bdc65c98bef9cd7a7b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>Δy</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a46e4b816757eb36919adcd8a8d3c299c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>Δy_always</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a560cef30409d81f0c2c2c0954f409ba6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>max_penalty_y</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>aff96dc973f14bd43277c46b5d39d3e18</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::CyQPALMBackend::Timings</name>
    <filename>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</filename>
    <anchor>structcyqlone_1_1qpalm_1_1CyQPALMBackend_1_1Timings</anchor>
    <member kind="typedef">
      <type>DefaultTimings</type>
      <name>type</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a1a4dc4a1734c726a442ef2e41a9862fa</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>guanaqo::Timed&lt; batmat::DefaultTimings &gt;</type>
      <name>timed_t</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>abe51e320bcf5ed3554d5eae33042f599</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>type</type>
      <name>breakpoints</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>af694ceffb0c3b4c5adbc12aebbfb56df</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>type</type>
      <name>calc_y_hat</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a16dea9b5b59872f315ee5999b4563976</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>type</type>
      <name>calc_y_hat_AT</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a90ab447f62a735ea3241498b07fdf12c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>type</type>
      <name>update_active_set_change</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a5bba09dad767dbd1942c9eb26e815e32</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>type</type>
      <name>update_factorization</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a24a908b457523bc2f4b15f5bfaf8da65</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>type</type>
      <name>factor</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a213dcf4f3a08a88ce2b981ab83e4af09</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>type</type>
      <name>solve</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a443290022a2449e43a253933aeae6e77</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>type</type>
      <name>solve_MT</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>ab3b8d53955512df2c1ebd0c16244a17d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>type</type>
      <name>solve_A</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>ad268450dea367c7e9443dd7c3817cb63</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>type</type>
      <name>solve_grad</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>adf86c39ae2d8e25f36839561cb2ae9d1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>type</type>
      <name>solve_resid</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a1d1fc41b74cddaa73a038e7b5993cc1f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>type</type>
      <name>recompute_outer_grad</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a53f3b86b882ba21cd935a8f4239076ec</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>type</type>
      <name>recompute_outer_A</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a33e2858e6d2cd7b0ec95e1336ba28541</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>type</type>
      <name>recompute_outer_AT</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>ab0d61a3df654d92123d871ba29507a53</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>type</type>
      <name>recompute_outer_MT</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>aa9ffc065fa11aac61ec9029e699be39c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>type</type>
      <name>recompute_outer_norm</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>ae59a438b41054d50128a85e69515d883</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>type</type>
      <name>recompute_inner_grad</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>ade1b78d1a2f74f465a81aca43bf642a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>type</type>
      <name>recompute_inner_A</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a4bb19e2f76538538a715c24cdc731e95</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>type</type>
      <name>recompute_inner_MT</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>aca4b6243bb26791d74915fa6471e3e26</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>type</type>
      <name>ineq_constr_resid</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>af24b3f3af397a739f53609a47de10fc1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>type</type>
      <name>ineq_constr_viol</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a0408adfc416f83ca3e035e38e5098f43</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>type</type>
      <name>ineq_constr_resid_al</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>afe87c80e422a2f9d1acef049270f2e2c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>type</type>
      <name>update_penalty_y</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend.html</anchorfile>
      <anchor>a3248c60e7e7484a222af16e8384c72a5</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::CyQPALMBackend::var_vec_t</name>
    <filename>structcyqlone_1_1qpalm_1_1CyQPALMBackend_1_1var__vec__t.html</filename>
    <member kind="function">
      <type></type>
      <name>var_vec_t</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend_1_1var__vec__t.html</anchorfile>
      <anchor>a181f6f39dfd08d0ab7ebeffee8bd5217</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="variable">
      <type>friend</type>
      <name>CyQPALMBackend</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend_1_1var__vec__t.html</anchorfile>
      <anchor>aa9e89f920dffda763f2626a3380d0768</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" protection="private">
      <type></type>
      <name>var_vec_t</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1CyQPALMBackend_1_1var__vec__t.html</anchorfile>
      <anchor>a82dac57d639bc33ff6b9795a69e7d4c7</anchor>
      <arglist>(storage_t &amp;&amp;o)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::CyQPALMBackendSettings</name>
    <filename>group__topic-optimization-solvers-ocp.html</filename>
    <anchor>structcyqlone_1_1qpalm_1_1CyQPALMBackendSettings</anchor>
    <member kind="variable">
      <type>index_t</type>
      <name>processors</name>
      <anchorfile>group__topic-optimization-solvers-ocp.html</anchorfile>
      <anchor>af00e70c02cd13e5ebf43d6638eb9df7c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>print_residuals</name>
      <anchorfile>group__topic-optimization-solvers-ocp.html</anchorfile>
      <anchor>a1ca10438c0578c7a0acec2f8e78d50b1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>print_precision</name>
      <anchorfile>group__topic-optimization-solvers-ocp.html</anchorfile>
      <anchor>add64f743497f10b2503b77de45c77c5d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>changing_constr_factor</name>
      <anchorfile>group__topic-optimization-solvers-ocp.html</anchorfile>
      <anchor>abd09901dbadec2708293d74a8ed78814</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>max_update_count</name>
      <anchorfile>group__topic-optimization-solvers-ocp.html</anchorfile>
      <anchor>acff95d8fedf3933328e02b7cc9023df8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>detailed_timings</name>
      <anchorfile>group__topic-optimization-solvers-ocp.html</anchorfile>
      <anchor>ac58824cc204ea83f5521f3f214d0f33a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>cyqlone::TricyqleParams&lt; real_t &gt;</type>
      <name>tricyqle_params</name>
      <anchorfile>group__topic-optimization-solvers-ocp.html</anchorfile>
      <anchor>ac8be3f5b5799e223800c1ca14cb28318</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>spin_count</name>
      <anchorfile>group__topic-optimization-solvers-ocp.html</anchorfile>
      <anchor>a7eae519b11db2e650190fd9a73476714</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>WarmStartingStrategy</type>
      <name>strategy</name>
      <anchorfile>group__topic-optimization-solvers-ocp.html</anchorfile>
      <anchor>ac58edb0fc69ddab30f2ffb3f6b903832</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::CyQPALMBackendStats</name>
    <filename>group__topic-optimization-solvers-ocp.html</filename>
    <anchor>structcyqlone_1_1qpalm_1_1CyQPALMBackendStats</anchor>
    <member kind="variable">
      <type>index_t</type>
      <name>num_updates</name>
      <anchorfile>group__topic-optimization-solvers-ocp.html</anchorfile>
      <anchor>ae3d57c48a5bcfe825b3100c5102c2c19</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>rank_updates</name>
      <anchorfile>group__topic-optimization-solvers-ocp.html</anchorfile>
      <anchor>af567449d5e242199aeb87353eccee6d8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>num_factor</name>
      <anchorfile>group__topic-optimization-solvers-ocp.html</anchorfile>
      <anchor>a7cb6ec6aec26e9353537758a13fe55d1</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::detail::backend_stats_type</name>
    <filename>namespacecyqlone_1_1qpalm_1_1detail.html</filename>
    <anchor>structcyqlone_1_1qpalm_1_1detail_1_1backend__stats__type</anchor>
    <templarg>class T</templarg>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::detail::backend_stats_type&lt; CyQPALMBackend&lt; VL, DefaultOrder &gt; &gt;</name>
    <filename>namespacecyqlone_1_1qpalm_1_1detail.html</filename>
    <anchor>structcyqlone_1_1qpalm_1_1detail_1_1backend__stats__type_3_01CyQPALMBackend_3_01VL_00_01DefaultOrder_01_4_01_4</anchor>
    <templarg>index_t VL</templarg>
    <templarg>StorageOrder DefaultOrder</templarg>
    <member kind="typedef">
      <type>CyQPALMBackendStats</type>
      <name>type</name>
      <anchorfile>namespacecyqlone_1_1qpalm_1_1detail.html</anchorfile>
      <anchor>a0a25bc568bbf94ed6cb8909c9d0c57a6</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::detail::backend_type</name>
    <filename>namespacecyqlone_1_1qpalm_1_1detail.html</filename>
    <anchor>structcyqlone_1_1qpalm_1_1detail_1_1backend__type</anchor>
    <templarg>class T</templarg>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::detail::backend_type&lt; std::unique_ptr&lt; T, D &gt; &gt;</name>
    <filename>namespacecyqlone_1_1qpalm_1_1detail.html</filename>
    <anchor>structcyqlone_1_1qpalm_1_1detail_1_1backend__type_3_01std_1_1unique__ptr_3_01T_00_01D_01_4_01_4</anchor>
    <templarg>class T</templarg>
    <templarg>class D</templarg>
    <member kind="typedef">
      <type>T</type>
      <name>type</name>
      <anchorfile>namespacecyqlone_1_1qpalm_1_1detail.html</anchorfile>
      <anchor>a313cd78907068fe910b6b5fdbfe0d449</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::detail::backend_type&lt; T * &gt;</name>
    <filename>namespacecyqlone_1_1qpalm_1_1detail.html</filename>
    <anchor>structcyqlone_1_1qpalm_1_1detail_1_1backend__type_3_01T_01_5_01_4</anchor>
    <templarg>class T</templarg>
    <member kind="typedef">
      <type>T</type>
      <name>type</name>
      <anchorfile>namespacecyqlone_1_1qpalm_1_1detail.html</anchorfile>
      <anchor>ad5954576da1a4ae6733dd297a48f020b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::detail::backend_type&lt; unique_CyQPALMBackend&lt; VL, DefaultOrder &gt; &gt;</name>
    <filename>namespacecyqlone_1_1qpalm_1_1detail.html</filename>
    <anchor>structcyqlone_1_1qpalm_1_1detail_1_1backend__type_3_01unique__CyQPALMBackend_3_01VL_00_01DefaultOrder_01_4_01_4</anchor>
    <templarg>index_t VL</templarg>
    <templarg>StorageOrder DefaultOrder</templarg>
    <member kind="typedef">
      <type>CyQPALMBackend&lt; VL, DefaultOrder &gt;</type>
      <name>type</name>
      <anchorfile>namespacecyqlone_1_1qpalm_1_1detail.html</anchorfile>
      <anchor>aeb581222383ce0eeb76780edebd95f1f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::DetailedStats</name>
    <filename>structcyqlone_1_1qpalm_1_1DetailedStats.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>ExitReason</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1DetailedStats.html</anchorfile>
      <anchor>afbba2d5a7368b173c108f104e7d7459c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Busy</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1DetailedStats.html</anchorfile>
      <anchor>afbba2d5a7368b173c108f104e7d7459ca31ddc63b44ee2a594e7254f88cb46a85</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Converged</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1DetailedStats.html</anchorfile>
      <anchor>afbba2d5a7368b173c108f104e7d7459ca4d1f8c96fbbe0840d849f750db3bbde8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NoActiveSetChange</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1DetailedStats.html</anchorfile>
      <anchor>afbba2d5a7368b173c108f104e7d7459ca6f8138db1d6c4dcc155498c9f0f65bed</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Fail</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1DetailedStats.html</anchorfile>
      <anchor>afbba2d5a7368b173c108f104e7d7459ca9e193bc9332f68e38de95c9f6d7dd471</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Busy</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1DetailedStats.html</anchorfile>
      <anchor>afbba2d5a7368b173c108f104e7d7459ca31ddc63b44ee2a594e7254f88cb46a85</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Converged</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1DetailedStats.html</anchorfile>
      <anchor>afbba2d5a7368b173c108f104e7d7459ca4d1f8c96fbbe0840d849f750db3bbde8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NoActiveSetChange</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1DetailedStats.html</anchorfile>
      <anchor>afbba2d5a7368b173c108f104e7d7459ca6f8138db1d6c4dcc155498c9f0f65bed</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Fail</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1DetailedStats.html</anchorfile>
      <anchor>afbba2d5a7368b173c108f104e7d7459ca9e193bc9332f68e38de95c9f6d7dd471</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; Entry &gt;</type>
      <name>entries</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1DetailedStats.html</anchorfile>
      <anchor>aeaa90761415eaeacbdf2468777bfd8c3</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::DetailedStats::Entry</name>
    <filename>structcyqlone_1_1qpalm_1_1DetailedStats.html</filename>
    <anchor>structcyqlone_1_1qpalm_1_1DetailedStats_1_1Entry</anchor>
    <member kind="variable">
      <type>unsigned</type>
      <name>outer_iter</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1DetailedStats.html</anchorfile>
      <anchor>a57d35d43c38512fc967735031f81e76a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>unsigned</type>
      <name>inner_iter</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1DetailedStats.html</anchorfile>
      <anchor>a0886a5a50477c181a18f62cff9a3cb8d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>stationarity</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1DetailedStats.html</anchorfile>
      <anchor>a6b569889ce1ed7b141bb4fd772d0d0d7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>ineq_constr_viol</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1DetailedStats.html</anchorfile>
      <anchor>ab79c9c65771dd1b5f9e2ff9f61fc1a62</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>eq_constr_viol</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1DetailedStats.html</anchorfile>
      <anchor>a943af382c674381df85a015a2ca6b116</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>linesearch_step_size</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1DetailedStats.html</anchorfile>
      <anchor>a4ae0e7ddc7f95c483730f6d18e94be1e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>linesearch_breakpoint_index</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1DetailedStats.html</anchorfile>
      <anchor>aa26bb4a0e27c9ae6d45799d45f34720f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>num_active_constr</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1DetailedStats.html</anchorfile>
      <anchor>a35efb9a45d591f0bd2ee9babaeed40d6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>num_changing_constr</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1DetailedStats.html</anchorfile>
      <anchor>abab412b8cbab6b98159a8e91f35d6dbc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>ExitReason</type>
      <name>exit_reason</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1DetailedStats.html</anchorfile>
      <anchor>a0aa2bbc01c8d299ab76df1fa09c0f3a0</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::get_breakpoints_fn</name>
    <filename>structcyqlone_1_1qpalm_1_1get__breakpoints__fn.html</filename>
    <member kind="typedef">
      <type>typename std::remove_cvref_t&lt; Backend &gt;::ineq_constr_vec_t</type>
      <name>vec_t</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1get__breakpoints__fn.html</anchorfile>
      <anchor>a1d6988dde4dea0259d78dbd5c93f5aaa</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>operator()</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1get__breakpoints__fn.html</anchorfile>
      <anchor>a4165fbfe60a5d0078d9cae44a6d5bb33</anchor>
      <arglist>(Backend &amp;backend, typename Backend::Context &amp;ctx, std::vector&lt; Breakpoint &gt; &amp;breakpoints, const vec_t&lt; Backend &gt; &amp;Σ, const vec_t&lt; Backend &gt; &amp;y, const vec_t&lt; Backend &gt; &amp;Ad, const vec_t&lt; Backend &gt; &amp;Ax, const vec_t&lt; Backend &gt; &amp;b_min, const vec_t&lt; Backend &gt; &amp;b_max) const noexcept(guanaqo::is_nothrow_tag_invocable_v&lt; get_breakpoints_fn, Backend &amp;, typename Backend::Context &amp;, std::vector&lt; Breakpoint &gt; &amp;, const vec_t&lt; Backend &gt; &amp;, const vec_t&lt; Backend &gt; &amp;, const vec_t&lt; Backend &gt; &amp;, const vec_t&lt; Backend &gt; &amp;, const vec_t&lt; Backend &gt; &amp;, const vec_t&lt; Backend &gt; &amp; &gt;)</arglist>
    </member>
    <member kind="function">
      <type>BreakpointsResult</type>
      <name>operator()</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1get__breakpoints__fn.html</anchorfile>
      <anchor>a5393edf9c40b1875f0cb8bb3d87467a0</anchor>
      <arglist>(Backend &amp;backend, typename Backend::Context &amp;ctx, std::vector&lt; Breakpoint &gt; &amp;breakpoints, const vec_t&lt; Backend &gt; &amp;Σ, const vec_t&lt; Backend &gt; &amp;y, const vec_t&lt; Backend &gt; &amp;Ad, const vec_t&lt; Backend &gt; &amp;Ax, const vec_t&lt; Backend &gt; &amp;b_min, const vec_t&lt; Backend &gt; &amp;b_max) const</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::get_partitioned_breakpoints_fn</name>
    <filename>structcyqlone_1_1qpalm_1_1get__partitioned__breakpoints__fn.html</filename>
    <member kind="typedef">
      <type>typename std::remove_cvref_t&lt; Backend &gt;::ineq_constr_vec_t</type>
      <name>vec_t</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1get__partitioned__breakpoints__fn.html</anchorfile>
      <anchor>a0113057ed2321d6dbbdc7532442a7bd7</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>PartitionedBreakpoints</type>
      <name>operator()</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1get__partitioned__breakpoints__fn.html</anchorfile>
      <anchor>aed34e0a8f6a9596d64bf614824f65dd1</anchor>
      <arglist>(Backend &amp;backend, typename Backend::Context &amp;ctx, std::vector&lt; Breakpoint &gt; &amp;breakpoints, const vec_t&lt; Backend &gt; &amp;Σ, const vec_t&lt; Backend &gt; &amp;y, const vec_t&lt; Backend &gt; &amp;Ad, const vec_t&lt; Backend &gt; &amp;Ax, const vec_t&lt; Backend &gt; &amp;b_min, const vec_t&lt; Backend &gt; &amp;b_max) const noexcept(guanaqo::is_nothrow_tag_invocable_v&lt; get_partitioned_breakpoints_fn, Backend &amp;, typename Backend::Context &amp;, std::vector&lt; Breakpoint &gt; &amp;, const vec_t&lt; Backend &gt; &amp;, const vec_t&lt; Backend &gt; &amp;, const vec_t&lt; Backend &gt; &amp;, const vec_t&lt; Backend &gt; &amp;, const vec_t&lt; Backend &gt; &amp;, const vec_t&lt; Backend &gt; &amp; &gt;)</arglist>
    </member>
    <member kind="function">
      <type>PartitionedBreakpoints</type>
      <name>operator()</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1get__partitioned__breakpoints__fn.html</anchorfile>
      <anchor>a54afee935b44ad846bab46b06c5a7023</anchor>
      <arglist>(Backend &amp;backend, typename Backend::Context &amp;ctx, std::vector&lt; Breakpoint &gt; &amp;breakpoints, const vec_t&lt; Backend &gt; &amp;Σ, const vec_t&lt; Backend &gt; &amp;y, const vec_t&lt; Backend &gt; &amp;Ad, const vec_t&lt; Backend &gt; &amp;Ax, const vec_t&lt; Backend &gt; &amp;b_min, const vec_t&lt; Backend &gt; &amp;b_max) const</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::LineSearch</name>
    <filename>structcyqlone_1_1qpalm_1_1LineSearch.html</filename>
    <templarg>class Vec</templarg>
    <member kind="typedef">
      <type>Vec</type>
      <name>vec_t</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1LineSearch.html</anchorfile>
      <anchor>a4011f0635bf0fa62ecae607bd1847b2d</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>Result</type>
      <name>operator()</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1LineSearch.html</anchorfile>
      <anchor>adabd73aeb68da24688d265bd83410c77</anchor>
      <arglist>(auto &amp;ctx, auto &amp;backend, real_t η, real_t β, const vec_t &amp;Σ, const vec_t &amp;y, const vec_t &amp;Ad, const vec_t &amp;Ax, const vec_t &amp;b_min, const vec_t &amp;b_max)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Result</type>
      <name>find_stepsize_base</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1LineSearch.html</anchorfile>
      <anchor>a797e329bf07df40fe5e812676f928f99</anchor>
      <arglist>(ABSum_t a, ABSum_t b, size_t i0, std::span&lt; Breakpoint &gt; pos_bp)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Result</type>
      <name>find_stepsize</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1LineSearch.html</anchorfile>
      <anchor>aa66bec6f6a6ef4001ebc7d36604f6a7e</anchor>
      <arglist>(ABSum_t a, ABSum_t b, size_t i0, std::span&lt; Breakpoint &gt; pos_bp, bool partition_1=true)</arglist>
    </member>
    <member kind="variable">
      <type>LineSearchSettings</type>
      <name>settings</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1LineSearch.html</anchorfile>
      <anchor>adbfceb401a1d883e631da54bbaf8d6a8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; Breakpoint &gt;</type>
      <name>breakpoints</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1LineSearch.html</anchorfile>
      <anchor>ad1e43b47a4edae60aedf187f6f1706b2</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::LineSearch::Result</name>
    <filename>structcyqlone_1_1qpalm_1_1LineSearch.html</filename>
    <anchor>structcyqlone_1_1qpalm_1_1LineSearch_1_1Result</anchor>
    <member kind="variable">
      <type>real_t</type>
      <name>τ</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1LineSearch.html</anchorfile>
      <anchor>a03019a07486a0ab6b644061f8d29851a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>index</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1LineSearch.html</anchorfile>
      <anchor>aa6c898ea0066772bb85a4a540b1feba5</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::LineSearchSettings</name>
    <filename>namespacecyqlone_1_1qpalm.html</filename>
    <anchor>structcyqlone_1_1qpalm_1_1LineSearchSettings</anchor>
    <member kind="variable">
      <type>bool</type>
      <name>find_smallest_breakpoint_first</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>ac2fff19946adc7531bc0afc11226ad8f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::PartitionedBreakpoints</name>
    <filename>namespacecyqlone_1_1qpalm.html</filename>
    <anchor>structcyqlone_1_1qpalm_1_1PartitionedBreakpoints</anchor>
    <member kind="variable">
      <type>std::span&lt; Breakpoint &gt;</type>
      <name>neg_bp</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a39e61a555be8f3e459906d7b57408662</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::span&lt; Breakpoint &gt;</type>
      <name>pos_bp</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a7ad314946fc51eb1d2a40f650b75ffbe</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::problems::PlatooningParams</name>
    <filename>namespacecyqlone_1_1qpalm_1_1problems.html</filename>
    <anchor>structcyqlone_1_1qpalm_1_1problems_1_1PlatooningParams</anchor>
    <member kind="variable">
      <type>real_t</type>
      <name>friction</name>
      <anchorfile>namespacecyqlone_1_1qpalm_1_1problems.html</anchorfile>
      <anchor>ac81cc8bbaa2da1b7342dfdb0d9bbbb93</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>F_max</name>
      <anchorfile>namespacecyqlone_1_1qpalm_1_1problems.html</anchorfile>
      <anchor>a71a4b628fd4a6dad01041e374a17a648</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>v_max</name>
      <anchorfile>namespacecyqlone_1_1qpalm_1_1problems.html</anchorfile>
      <anchor>a2f8210dc4646636458fbe795d639fc68</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>dist_min</name>
      <anchorfile>namespacecyqlone_1_1qpalm_1_1problems.html</anchorfile>
      <anchor>abd5d627970536543324be1a34283e96c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>dist_init</name>
      <anchorfile>namespacecyqlone_1_1qpalm_1_1problems.html</anchorfile>
      <anchor>a637adefd6c549cd259f10f7d12118539</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>p_target</name>
      <anchorfile>namespacecyqlone_1_1qpalm_1_1problems.html</anchorfile>
      <anchor>afa44560fbbbbd9e72ceb56b2c68a083a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>N_horiz</name>
      <anchorfile>namespacecyqlone_1_1qpalm_1_1problems.html</anchorfile>
      <anchor>a99b5506c96a0fee3aa95cdef7f3d92d3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>T_horiz</name>
      <anchorfile>namespacecyqlone_1_1qpalm_1_1problems.html</anchorfile>
      <anchor>a146ecce5b3ebc178809def4438370946</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>scale_cost</name>
      <anchorfile>namespacecyqlone_1_1qpalm_1_1problems.html</anchorfile>
      <anchor>a278998f75a9a28795fa59467492856e7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; real_t &gt;</type>
      <name>masses</name>
      <anchorfile>namespacecyqlone_1_1qpalm_1_1problems.html</anchorfile>
      <anchor>a0b43e23740be9973cabb06551fa569a7</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::problems::PlatooningProblem</name>
    <filename>namespacecyqlone_1_1qpalm_1_1problems.html</filename>
    <anchor>structcyqlone_1_1qpalm_1_1problems_1_1PlatooningProblem</anchor>
    <member kind="variable">
      <type>LinearOCPStorage</type>
      <name>ocp</name>
      <anchorfile>namespacecyqlone_1_1qpalm_1_1problems.html</anchorfile>
      <anchor>afaad2533c84ecf56d28b0848410ca29f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; real_t &gt;</type>
      <name>ref</name>
      <anchorfile>namespacecyqlone_1_1qpalm_1_1problems.html</anchorfile>
      <anchor>a919449a70bfca330f0c99653129867e1</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::problems::SpringMassParams</name>
    <filename>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>ActuatorPlacement</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a18d1032c9ae1e2b5bbe9df30c50413f1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IndividualActuators</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a18d1032c9ae1e2b5bbe9df30c50413f1ac68a5dc9e1917b565380d1ab6e675849</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>RandomActuators</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a18d1032c9ae1e2b5bbe9df30c50413f1a30d42f40d22544857de9a609caf07ae2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>RandomPairsOfActuators</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a18d1032c9ae1e2b5bbe9df30c50413f1a617eba72fd8a383f9f58874e67c9e912</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>WangBoydActuators</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a18d1032c9ae1e2b5bbe9df30c50413f1a9ed41af8dca64bf2fe5d84464f710060</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IndividualActuators</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a18d1032c9ae1e2b5bbe9df30c50413f1ac68a5dc9e1917b565380d1ab6e675849</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>RandomActuators</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a18d1032c9ae1e2b5bbe9df30c50413f1a30d42f40d22544857de9a609caf07ae2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>RandomPairsOfActuators</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a18d1032c9ae1e2b5bbe9df30c50413f1a617eba72fd8a383f9f58874e67c9e912</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>WangBoydActuators</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a18d1032c9ae1e2b5bbe9df30c50413f1a9ed41af8dca64bf2fe5d84464f710060</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>static SpringMassParams</type>
      <name>wang_boyd_2008</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>ac8f9cdf3a81a4bd2ff3b8f01143035af</anchor>
      <arglist>(index_t n_masses, index_t N_horiz=30, uint64_t seed=0)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static SpringMassParams</type>
      <name>wang_boyd_2008_width</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a2c47e72483a5afacffa1fb7449ede6b5</anchor>
      <arglist>(index_t n_masses, index_t N_horiz=30, uint64_t seed=0, double steady_state_spring_length=0.1)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static SpringMassParams</type>
      <name>domahidi_2012</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>ac85087e18caea8b4d4b755e273ae740d</anchor>
      <arglist>(index_t n_masses, index_t N_horiz, uint64_t seed=0)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static SpringMassParams</type>
      <name>active_state_constr</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>abc65cdb2b4298fb3a0324b5aeaa81e8f</anchor>
      <arglist>(index_t n_masses=18, index_t N_horiz=256, uint64_t seed=0)</arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>friction</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a1dba013a3d54b283f9a36f97bd5f1f2e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>k_spring</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>ab5a2af96e4f38127c9e16a4581b86679</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>F_max</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a522c2f182381139c34e576be1652e0d9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>p_max</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>ae51c660af8c130707f451dde118e4a3c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>p_min</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a610a7a0c64707d230b7f120570aaf41f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>p_min_f</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a70e147415444754a12f9d828c7a2d6d6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>p_max_f</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a20401c1d408994186ca5f1316b346f40</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>v_max</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a6a7aac80e23a91f8e1861dec691d5a03</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>v_max_f</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a6a89e4327b4809fdf0b0e61f111c2dfe</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>width</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a887fc5679d33abe3be614792cddffd85</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>N_horiz</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>af6edc6dec7fa7b2a9e15766d9748f9b4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>T_horiz</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a202950abd5806db1f1dec39c2bb3d51b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>q_vel</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a9fbc86d261ebb7bdb01a7f37a58d0174</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>q_pos</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>af8a2fd9a992b8305847a25fd24c9d1bd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>q_vel_f</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a5443953a7bc84041776f986b5fe0e519</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>q_pos_f</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>ac567b9cf366b0a724de6ac966e915e89</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>r_act</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a1ba21c87e90b5089efb04b787d5b4da9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; real_t &gt;</type>
      <name>masses</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a0a29b4bcbbbcb45b1874cd956a811071</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>n_actuators</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>acfc2b8ef98e88e9b219e1eedd13f4b7b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>enum cyqlone::qpalm::problems::SpringMassParams::ActuatorPlacement</type>
      <name>actuator_placement</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>ac172425aa0e56c2a6db220e71e700b3e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint64_t</type>
      <name>seed</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a6c6b090dc45b6c7e373a408fbefdbe54</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::problems::SpringMassProblem</name>
    <filename>namespacecyqlone_1_1qpalm_1_1problems.html</filename>
    <anchor>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassProblem</anchor>
    <member kind="variable">
      <type>LinearOCPStorage</type>
      <name>ocp</name>
      <anchorfile>namespacecyqlone_1_1qpalm_1_1problems.html</anchorfile>
      <anchor>a5c10891cab55f962cdb9e2679df4b2ce</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; real_t &gt;</type>
      <name>ref</name>
      <anchorfile>namespacecyqlone_1_1qpalm_1_1problems.html</anchorfile>
      <anchor>a9bfbe7b1b19fbc1fe7d5ffa8c7234910</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::Settings</name>
    <filename>structcyqlone_1_1qpalm_1_1Settings.html</filename>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>a9be8b3eb59c9df5b7ca7120b7410c3e6</anchor>
      <arglist>(const Settings &amp;) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator!=</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>aaf03048cf1bf06f149ea361ed41c88a0</anchor>
      <arglist>(const Settings &amp;other) const</arglist>
    </member>
    <member kind="variable">
      <type>unsigned</type>
      <name>max_outer_iter</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>ac9090bf9b9298054631ea7d6c3a693e0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>unsigned</type>
      <name>max_inner_iter</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>adbca66e89cf0e36d35759f4f7242f084</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>unsigned</type>
      <name>max_total_inner_iter</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>a515ecca5d8342d9ad0c09581358efa8c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::chrono::microseconds</type>
      <name>max_time</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>af78fad11450c21eeb0dc2a4e27882a1e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>tolerance</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>a604567d806df4d6ec9ab2522f3ca803d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>dual_tolerance</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>a91a765126c9120f79f40d4a597640c8c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>eq_constr_tolerance</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>ad66071896a3caab2e37080c77cc4a343</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>initial_inner_tolerance</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>a42592dae1b8a1ac2ed162bc429b1d811</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>ρ</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>a8db61e7713f34a508a71e97b67217c20</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>θ</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>ab5d863568d7480e236b7afc32ce7521e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>Δy</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>a78e7361dbb3e5379436bd46f43414d2f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>Δy_always</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>a36fef6363ab0f0de8e9bf7cdd8380265</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>max_penalty_y</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>a972306dc31bec781d356b2d746c8fc3f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>initial_penalty_y</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>a0dd70fdbec05ca3053d9ea475a227d68</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>scale_initial_penalty_y</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>a89ccde4b8379e5f343154eba11ef695f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>Δx</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>aa15b0558ea04a215d1758fedf5d6cd41</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>max_penalty_x</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>a4e42bf80414aec6ce52346ebe541254f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>boost_penalty_x</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>ac46e5da8b5e1033ffee4c50b1b888878</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>initial_penalty_x</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>a010943e66e81c08f22df67ac7d61d4e6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>proximal</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>a822880513814f61b212eb176f4c344ed</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>recompute_eq_res</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>aaf2ccd8782df804b34d5c54836de5fd6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>recompute_inner</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>a4ac0f331a56105f5eaf8e5c0d7dbd592</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>recompute</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>adeeed2f5ebe50223962d3d0f45fa8677</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>verbose</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>a25954ef9b610d1d58103257ae20032b1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>print_precision</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>a056c1f3656deb4bb4a251b863c52a055</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>unsigned</type>
      <name>max_no_changes_active_set</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>a0e6123400ca8df7f5b663973ae64c00a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>linesearch_include_multipliers</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>a1c2e46b3ed3272228a765d1f25780e26</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>force_linesearch_if_no_set_change</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>a7063860509c616716b1e4cb0b9218d95</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>force_linesearch_if_dir_deriv_pos</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>a51cfcc03495f12d5a186c763a356b772</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>detailed_stats</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>a59257ea29ecdfc3d349696d4d75ed2f7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>scale_newton_step</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>af6ed344a2af61f08695210cea0cf120f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>print_directional_deriv</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>a2af53ebb7f2b0201286ed7533457793c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>print_linesearch_inputs</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1Settings.html</anchorfile>
      <anchor>a1e5fd50f3364cb464a53f0b215f6c577</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>cyqlone::qpalm::Solver</name>
    <filename>classcyqlone_1_1qpalm_1_1Solver.html</filename>
    <templarg>class Backend</templarg>
    <member kind="typedef">
      <type>typename detail::backend_stats_type&lt; backend_type &gt;::type</type>
      <name>BackendStats</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>a5e9be025d398f7777aada3e214e9f72e</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>SolverStatus</type>
      <name>operator()</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>a360bacd7bd69d857de551f6afa807583</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>get_num_variables</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>ade67a39b7de8d754c6b34516913b94f4</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>get_num_equality_constraints</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>a3d34b4bf63cf4a64d75a28661b18ad3e</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>get_num_inequality_constraints</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>a9087fec3b36d2e7795555e097b81ae56</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>has_result</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>aaf903dfe0171e1f52b7b3be5a8ab5c3a</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>get_solution</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>ae4d201b79ee15c1fb9a5be2128a04c68</anchor>
      <arglist>(std::span&lt; real_t &gt;) const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; real_t &gt;</type>
      <name>get_solution</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>aa641b1c16589f7957f78ac175eb5000b</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>get_equality_multipliers</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>a4884fb12e75d2ca2667e9c6044201e5d</anchor>
      <arglist>(std::span&lt; real_t &gt;) const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; real_t &gt;</type>
      <name>get_equality_multipliers</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>a8b96f0f22f26395472962929b9549c5f</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>get_equality_constraints</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>a6dc0e385224505aeb966c670a4a7e2d3</anchor>
      <arglist>(std::span&lt; real_t &gt;) const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; real_t &gt;</type>
      <name>get_equality_constraints</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>aaa6ce03c9fa6566942e784d0d7b04881</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>get_inequality_multipliers</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>ac03face48fec747837c37f794b025de3</anchor>
      <arglist>(std::span&lt; real_t &gt;) const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; real_t &gt;</type>
      <name>get_inequality_multipliers</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>a0a41c7830f109e5c8a0594652bfd2410</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>get_inequality_constraints</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>a40effc9d8e12effae34d392e68ee16a6</anchor>
      <arglist>(std::span&lt; real_t &gt;) const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; real_t &gt;</type>
      <name>get_inequality_constraints</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>abc8b439ef78d256d68b9db2ed5541256</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>get_penalty_factors</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>a17fb04d7426c7f4bd11c49000c8f9a1d</anchor>
      <arglist>(std::span&lt; real_t &gt;) const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; real_t &gt;</type>
      <name>get_penalty_factors</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>aeeaae1d4f38af2eeeb232295ff00b188</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>warm_start_solution</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>abaef224fb463ba3a15bfce92eabc94ca</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_initial_guess</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>add9ec6bd553362b1445da9234e3b7899</anchor>
      <arglist>(std::span&lt; const real_t &gt; x, std::span&lt; const real_t &gt; y, std::span&lt; const real_t &gt; λ)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_initial_guess</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>a088f56e2c4b01879da588ff5e108ec9a</anchor>
      <arglist>(std::span&lt; real_t &gt; x, std::span&lt; real_t &gt; y, std::span&lt; real_t &gt; λ)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_b_eq</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>ad3663285ee0b100080181b4d5e5c05a6</anchor>
      <arglist>(std::span&lt; const real_t &gt; b_eq)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_b_lb</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>af0fd34bfe915e89b65edb57a42481756</anchor>
      <arglist>(std::span&lt; const real_t &gt; b_lb)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_b_ub</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>a164292e938142aea28bd9dffa3663d64</anchor>
      <arglist>(std::span&lt; const real_t &gt; b_ub)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>stop</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>a62db437a880e4ca927fe045a1d5ac87b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Solver</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>abbec795f190d8aae0f4ccfa968bd7264</anchor>
      <arglist>(Backend backend, Settings settings={})</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Solver</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>a2473838de538776658d6b1cd425c2fa7</anchor>
      <arglist>(const Solver &amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type>Solver &amp;</type>
      <name>operator=</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>a683f2f96c504bd9a62a71a63577845b9</anchor>
      <arglist>(const Solver &amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Solver</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>a2765ef900d1635c20eb7a9d3bdbf0082</anchor>
      <arglist>(Solver &amp;&amp;) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Solver &amp;</type>
      <name>operator=</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>ae1c62e390d6ef043d3b82c9095c09b44</anchor>
      <arglist>(Solver &amp;&amp;) noexcept</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~Solver</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>aba52d3c92fafceb6fe39f937f2d73db3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable">
      <type>Backend</type>
      <name>backend</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>af80c04f3638569ca5961d5c4f249e5bf</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Settings</type>
      <name>settings</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>a955cf259b7871f63bea33202bc7bdd50</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::unique_ptr&lt; SolverImplementation&lt; backend_type &gt; &gt;</type>
      <name>impl</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>aa9562b90a001b01a7c788b7acacd7699</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::optional&lt; SolverStats &gt;</type>
      <name>stats</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>a2d775e3121d52a669d227b51598e0a65</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::optional&lt; BackendStats &gt;</type>
      <name>stats_backend</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>a0dd8dbd96e6fedf89aa1180b018f6dc3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>guanaqo::AtomicStopSignal</type>
      <name>stop_signal</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>a2231913f0bf83c03742ca76e1525e15f</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef" protection="private">
      <type>detail::backend_type_t&lt; Backend &gt;</type>
      <name>backend_type</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>a351d10d7e746d5ceb6fc6ebbb57c01cb</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" protection="private">
      <type>SolverStatus</type>
      <name>do_solve</name>
      <anchorfile>classcyqlone_1_1qpalm_1_1Solver.html</anchorfile>
      <anchor>a86283f73247832e1c17ea3c639d99d6c</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::SolverImplementation</name>
    <filename>structcyqlone_1_1qpalm_1_1SolverImplementation.html</filename>
    <templarg>class Backend</templarg>
    <member kind="typedef">
      <type>Backend</type>
      <name>backend_type</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>ad3d5e547fe54253a35c503000ae9957f</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename backend_type::active_set_t</type>
      <name>active_set_t</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>a67c434f22cfab91fc164db8d9c116780</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename backend_type::ineq_constr_vec_t</type>
      <name>ineq_vec_t</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>ae3ba776437c40d7b7b449ff25736ad69</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename backend_type::eq_constr_vec_t</type>
      <name>eq_vec_t</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>a258af9d0152cbaa9462f000a7fea9485</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename backend_type::var_vec_t</type>
      <name>var_vec_t</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>a5a323dfa6e6ff3b8585c4b498c6b8344</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>ensure_storage</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>a5f7017998c8b3dce13690968fd2d6299</anchor>
      <arglist>(backend_type &amp;backend)</arglist>
    </member>
    <member kind="function">
      <type>SolverStatus</type>
      <name>do_main_loop</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>acebb587f363542055ed61f0b8c1fb8f1</anchor>
      <arglist>(Backend::Context &amp;ctx, backend_type &amp;backend, const Settings &amp;settings, guanaqo::AtomicStopSignal &amp;stop_signal, SolverStats &amp;stats)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>initialize_penalty_y</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>a30930ad39860699cc18f15336f6c93ce</anchor>
      <arglist>(Backend::Context &amp;ctx, backend_type &amp;backend, real_t f0, const ineq_vec_t &amp;e0, ineq_vec_t &amp;Σ, const Settings &amp;settings)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static index_t</type>
      <name>update_penalty_y</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>aeca28f0766eff6405b150db702661a84</anchor>
      <arglist>(Backend::Context &amp;ctx, backend_type &amp;backend, ineq_vec_t &amp;Σ, const ineq_vec_t &amp;e, const ineq_vec_t &amp;e_old, const Settings &amp;settings)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static real_t</type>
      <name>update_penalty_x</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>ae03cb5a76ddf3129488cf00fedaeef86</anchor>
      <arglist>(real_t S, const Settings &amp;settings)</arglist>
    </member>
    <member kind="variable">
      <type>LineSearch&lt; ineq_vec_t &gt;</type>
      <name>linesearch</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>acd99d245f212c2a5153d3e21a53a5cac</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>active_set_t</type>
      <name>active_set</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>a133ce19074efa6bb16cea9a55366cf6a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>active_set_t</type>
      <name>active_set_old</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>a5a4bb430d59135bbd9ab8fd69872708e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>ineq_vec_t</type>
      <name>Σ</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>a1b6ed7b638b9abbb88cc0a92d9dc6c92</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>ineq_vec_t</type>
      <name>y</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>ae82f3298e1e256446ccc50b11626dbfd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>ineq_vec_t</type>
      <name>ŷ</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>a625329a85eff1a9c81ecb06e4827b30e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>ineq_vec_t</type>
      <name>e</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>a889230a838400fdd08d3e7d9e4572ee6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>ineq_vec_t</type>
      <name>e_old</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>a044ad1db552d3890866688c9e5e18990</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>ineq_vec_t</type>
      <name>Ax</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>ae9d970a82cac621bcba89780d7ff44c6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>ineq_vec_t</type>
      <name>Ad</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>a3c34534a1bfce2131c879e01557863e1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>eq_vec_t</type>
      <name>Mxb</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>aafaac7519adab2070c51345ef6e486c5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>eq_vec_t</type>
      <name>Δλ</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>aab33b05f7516d390babbeb04156254f0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>eq_vec_t</type>
      <name>λ</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>a6437ff2c50e1b6596ae3fb583bb4ec98</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>var_vec_t</type>
      <name>x</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>aa5b94cb580a4522b76f5c2357f87b605</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>var_vec_t</type>
      <name>grad</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>ac2356290b62119851ff68de07515456f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>var_vec_t</type>
      <name>Mᵀλ</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>a502d8ea367f2203afbf19d085b7f7f54</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>var_vec_t</type>
      <name>Aᵀŷ</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>ac406e699106ea94ba60acbf5b7e603ce</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>var_vec_t</type>
      <name>x_outer</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>a6d6cc97ac2911bbe32c1d2b64f21785c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>var_vec_t</type>
      <name>MᵀΔλ</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>a11cd80b2b9eb33af400c739418adc387</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>var_vec_t</type>
      <name>d</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>ad343e19fdb19ad8e3a5cc83c80b48885</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>var_vec_t</type>
      <name>ξ</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>a0698b9f0bb505542ff75fd3e8a25e46b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>var_vec_t</type>
      <name>grad_add</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverImplementation.html</anchorfile>
      <anchor>a2222f8611460ceb4869e61869c82c486</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::SolverStats</name>
    <filename>group__topic-optimization-solvers.html</filename>
    <anchor>structcyqlone_1_1qpalm_1_1SolverStats</anchor>
    <member kind="variable">
      <type>unsigned</type>
      <name>inner_iter</name>
      <anchorfile>group__topic-optimization-solvers.html</anchorfile>
      <anchor>adc75854df050f2e03ab9e518d99baade</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>unsigned</type>
      <name>outer_iter</name>
      <anchorfile>group__topic-optimization-solvers.html</anchorfile>
      <anchor>ad44c570bc854d2d3e6dd8fc5f81e4791</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>stationarity</name>
      <anchorfile>group__topic-optimization-solvers.html</anchorfile>
      <anchor>a211599a03a96d0763a347a5a2369f735</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>primal_residual_norm</name>
      <anchorfile>group__topic-optimization-solvers.html</anchorfile>
      <anchor>a7e0f4e611db03ea5ef10ec00c14bc118</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>max_penalty</name>
      <anchorfile>group__topic-optimization-solvers.html</anchorfile>
      <anchor>ab6b5e84cd3ad52c8e1c1cad930e896ed</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>SolverTimings</type>
      <name>timings</name>
      <anchorfile>group__topic-optimization-solvers.html</anchorfile>
      <anchor>a583afdaf6f7dea1a0bdb16f7827918dc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::optional&lt; DetailedStats &gt;</type>
      <name>detail</name>
      <anchorfile>group__topic-optimization-solvers.html</anchorfile>
      <anchor>a5d12e93e75443d9cee74c8f774bb0dd6</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::SolverTimings</name>
    <filename>structcyqlone_1_1qpalm_1_1SolverTimings.html</filename>
    <member kind="variable">
      <type>DefaultTimings</type>
      <name>total</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverTimings.html</anchorfile>
      <anchor>af05f866344d2c8d0b431bf82e6bcb528</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>DefaultTimings</type>
      <name>scaling</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverTimings.html</anchorfile>
      <anchor>a6817bb47e1b906671e2ff3fc539df997</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>DefaultTimings</type>
      <name>line_search</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverTimings.html</anchorfile>
      <anchor>a5e598eb1eb75831ade2d39c6ef763962</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>DefaultTimings</type>
      <name>recompute_inner</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverTimings.html</anchorfile>
      <anchor>a204e1c7d89db46c34718a46fbcab5c31</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>DefaultTimings</type>
      <name>recompute_outer</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverTimings.html</anchorfile>
      <anchor>ac362d870927e569e86a85b5afc233a71</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>DefaultTimings</type>
      <name>mat_vec_M</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverTimings.html</anchorfile>
      <anchor>a5e3650cb9a7f98e5d8bc82b52d9d2c6b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>DefaultTimings</type>
      <name>mat_vec_MT</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverTimings.html</anchorfile>
      <anchor>aa2d2311d3ddcf436aa8e801ed960e814</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>DefaultTimings</type>
      <name>mat_vec_A</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverTimings.html</anchorfile>
      <anchor>a1155e1ba338cbaf461369dae46d03068</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>DefaultTimings</type>
      <name>mat_vec_AT</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverTimings.html</anchorfile>
      <anchor>a5f414bd26fea73b4cb14516d253cd216</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>DefaultTimings</type>
      <name>mat_vec_Q</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverTimings.html</anchorfile>
      <anchor>a48f490b0bfaef5afa76af09bc2a81984</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>DefaultTimings</type>
      <name>active_set_change</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverTimings.html</anchorfile>
      <anchor>a7b8a81360c87e947b2e7b6d7caf96e43</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>DefaultTimings</type>
      <name>update_penalty</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverTimings.html</anchorfile>
      <anchor>a2e5f2c5a7c88fd2d3c52662e0357ba12</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>DefaultTimings</type>
      <name>update_regularization</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverTimings.html</anchorfile>
      <anchor>a64b885dd8e1d0d6806ca3067038154f0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>DefaultTimings</type>
      <name>boost_regularization</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverTimings.html</anchorfile>
      <anchor>a3525a9be60e61aaa7249a8aa45b10d6f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>DefaultTimings</type>
      <name>solve</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverTimings.html</anchorfile>
      <anchor>a60979bac16c487ca041325a8aebe0f08</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::map&lt; std::string, DefaultTimings &gt;</type>
      <name>backend</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverTimings.html</anchorfile>
      <anchor>a1f2d0413d9e3d80270f2674513876e51</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>std::ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1SolverTimings.html</anchorfile>
      <anchor>a3177822c4e8bd78867cb8736f643e562</anchor>
      <arglist>(std::ostream &amp;, const SolverTimings &amp;)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::qpalm::unique_CyQPALMBackend</name>
    <filename>structcyqlone_1_1qpalm_1_1unique__CyQPALMBackend.html</filename>
    <templarg>index_t VL</templarg>
    <templarg>StorageOrder DefaultOrder</templarg>
    <member kind="function">
      <type></type>
      <name>unique_CyQPALMBackend</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1unique__CyQPALMBackend.html</anchorfile>
      <anchor>ae48c54f7523a4d8151b13be51204696c</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>unique_CyQPALMBackend</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1unique__CyQPALMBackend.html</anchorfile>
      <anchor>a2705bfe5f1f82c6ab1f4202c3f27acc7</anchor>
      <arglist>(unique_CyQPALMBackend &amp;&amp;) noexcept=default</arglist>
    </member>
    <member kind="function">
      <type>unique_CyQPALMBackend &amp;</type>
      <name>operator=</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1unique__CyQPALMBackend.html</anchorfile>
      <anchor>a53de35e407e92d4b9a931ba698524ee2</anchor>
      <arglist>(unique_CyQPALMBackend &amp;&amp;) noexcept=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~unique_CyQPALMBackend</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1unique__CyQPALMBackend.html</anchorfile>
      <anchor>af371fc8c95ce721f843746a819929a2e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>unique_CyQPALMBackend</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1unique__CyQPALMBackend.html</anchorfile>
      <anchor>aac81ea68f1f225346bf8001358fb6fd4</anchor>
      <arglist>(std::unique_ptr&lt; CyQPALMBackend&lt; VL, DefaultOrder &gt; &gt; &amp;&amp;o) noexcept</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::SparseCOO</name>
    <filename>structcyqlone_1_1SparseCOO.html</filename>
    <templarg>class class Index</templarg>
  </compound>
  <compound kind="struct">
    <name>cyqlone::SparseCSC</name>
    <filename>structcyqlone_1_1SparseCSC.html</filename>
    <templarg>class class Index</templarg>
    <templarg>class class StorageIndex</templarg>
  </compound>
  <compound kind="struct">
    <name>cyqlone::SparseMatrix</name>
    <filename>structcyqlone_1_1SparseMatrix.html</filename>
    <member kind="function">
      <type>auto</type>
      <name>iter_coo</name>
      <anchorfile>structcyqlone_1_1SparseMatrix.html</anchorfile>
      <anchor>af045ed16c8458b849c5d5efd78dcc614</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; index_t &gt;</type>
      <name>row_indices</name>
      <anchorfile>structcyqlone_1_1SparseMatrix.html</anchorfile>
      <anchor>a4d79e8a7c7688a17c4989b2a8f44a075</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; index_t &gt;</type>
      <name>col_indices</name>
      <anchorfile>structcyqlone_1_1SparseMatrix.html</anchorfile>
      <anchor>aff52eb48294d9712d48d795ea08cd413</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; real_t &gt;</type>
      <name>values</name>
      <anchorfile>structcyqlone_1_1SparseMatrix.html</anchorfile>
      <anchor>affcc16b1597ec5f4726e480f16afecc0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const SparseCOO&lt; index_t &gt;</type>
      <name>sparsity</name>
      <anchorfile>structcyqlone_1_1SparseMatrix.html</anchorfile>
      <anchor>a15da14f54f507c2f4f3d99cc835dea77</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::SparseMatrixBuilder</name>
    <filename>structcyqlone_1_1SparseMatrixBuilder.html</filename>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>structcyqlone_1_1SparseMatrixBuilder.html</anchorfile>
      <anchor>a8ab034246635fec83768f94488038064</anchor>
      <arglist>(index_t row, index_t col, real_t value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>structcyqlone_1_1SparseMatrixBuilder.html</anchorfile>
      <anchor>ad993733a1dec0503e7673f660a4bad9d</anchor>
      <arglist>(index_t row, index_t col, guanaqo::MatrixView&lt; T, I, S, O &gt; dense, std::remove_cvref_t&lt; T &gt; scale=1, batmat::linalg::MatrixStructure structure=batmat::linalg::MatrixStructure::General)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_diag</name>
      <anchorfile>structcyqlone_1_1SparseMatrixBuilder.html</anchorfile>
      <anchor>a41d1b610717191be7fc1e42be93a0b39</anchor>
      <arglist>(index_t row, index_t col, real_t value, index_t n)</arglist>
    </member>
    <member kind="function">
      <type>SparseMatrix</type>
      <name>build</name>
      <anchorfile>structcyqlone_1_1SparseMatrixBuilder.html</anchorfile>
      <anchor>a7fa11a02b1f0e55d4df17d4373263fbb</anchor>
      <arglist>() const &amp;</arglist>
    </member>
    <member kind="function">
      <type>SparseMatrix</type>
      <name>build</name>
      <anchorfile>structcyqlone_1_1SparseMatrixBuilder.html</anchorfile>
      <anchor>afc8fc02520b34d299e508991371f9eb2</anchor>
      <arglist>() &amp;&amp;</arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>rows</name>
      <anchorfile>structcyqlone_1_1SparseMatrixBuilder.html</anchorfile>
      <anchor>a2c9945ed6621b2501b848abcc1cd43e3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>cols</name>
      <anchorfile>structcyqlone_1_1SparseMatrixBuilder.html</anchorfile>
      <anchor>ab250de50d86e2de15dfcf4754f99ca2e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Symmetry</type>
      <name>symmetry</name>
      <anchorfile>structcyqlone_1_1SparseMatrixBuilder.html</anchorfile>
      <anchor>a19ecbe0cd231677dec36961eb9f6c2f5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; index_t &gt;</type>
      <name>row_indices</name>
      <anchorfile>structcyqlone_1_1SparseMatrixBuilder.html</anchorfile>
      <anchor>acac935b09002da232819e3fc1b7ceacd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; index_t &gt;</type>
      <name>col_indices</name>
      <anchorfile>structcyqlone_1_1SparseMatrixBuilder.html</anchorfile>
      <anchor>ae1d6f63145da26847a8c75cc4ffa04fc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; real_t &gt;</type>
      <name>values</name>
      <anchorfile>structcyqlone_1_1SparseMatrixBuilder.html</anchorfile>
      <anchor>ad94400ad0a7d0ccc8d2d0a26b2898abc</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>cyqlone::TreeBarrier</name>
    <filename>classcyqlone_1_1TreeBarrier.html</filename>
    <templarg>typename CompletionFn</templarg>
    <templarg>class PhaseType</templarg>
    <class kind="class">cyqlone::TreeBarrier::arrival_token</class>
    <class kind="class">cyqlone::TreeBarrier::arrival_token_typed</class>
    <class kind="struct">cyqlone::TreeBarrier::Storage</class>
    <class kind="struct">cyqlone::TreeBarrier::State</class>
    <member kind="enumeration">
      <type></type>
      <name>BarrierPhase</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>a50fd92d9bf505a3e94b41afc10beeb41</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TreeBarrier</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>a198aa3c6d872f16ce45755eee389dc6b</anchor>
      <arglist>(uint32_t expected, CompletionFn completion)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TreeBarrier</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>a458a9a0e4e44c1db3c643cbe1a222025</anchor>
      <arglist>(const TreeBarrier &amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TreeBarrier</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>a5a7ca6198a9a8d0348737bf58a28c944</anchor>
      <arglist>(TreeBarrier &amp;&amp;)=default</arglist>
    </member>
    <member kind="function">
      <type>TreeBarrier &amp;</type>
      <name>operator=</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>ac9175faa019c3f367f51582fabc85bbd</anchor>
      <arglist>(const TreeBarrier &amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type>TreeBarrier &amp;</type>
      <name>operator=</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>ab2d8f9aee39fe0dd293ce02f87411321</anchor>
      <arglist>(TreeBarrier &amp;&amp;)=default</arglist>
    </member>
    <member kind="function">
      <type>arrival_token</type>
      <name>arrive_with_completion</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>a7411730540f4e108e50ae9075dd0324b</anchor>
      <arglist>(uint32_t thread_id, C &amp;&amp;custom_completion)</arglist>
    </member>
    <member kind="function">
      <type>arrival_token</type>
      <name>arrive</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>a4c337b67cc8d868958fef5acc11b7e74</anchor>
      <arglist>(uint32_t thread_id)</arglist>
    </member>
    <member kind="function">
      <type>arrival_token</type>
      <name>arrive</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>aa5776a8bc40dbc6cae4188494ebde10a</anchor>
      <arglist>(uint32_t thread_id, int line)</arglist>
    </member>
    <member kind="function">
      <type>BarrierPhase</type>
      <name>current_phase</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>a3daea9482b5a6a0c24eaf5e5b9bf5543</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>wait_may_block</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>a20da7a78bcb74b76631517918cc84953</anchor>
      <arglist>(const arrival_token &amp;token) const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>wait</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>a863cb1f12340ccb2a5cd4a2825956334</anchor>
      <arglist>(arrival_token &amp;&amp;token) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>arrive_and_wait</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>ac491f75b3a472594fc122dc4fbdbac99</anchor>
      <arglist>(uint32_t thread_id)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>arrive_and_wait</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>a456977b149d9c8af2938ae2271f06f68</anchor>
      <arglist>(uint32_t thread_id, int line)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>arrive_and_wait_with_completion</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>a209bc761da78fc11ba9187781efae9ab</anchor>
      <arglist>(uint32_t thread_id, C &amp;&amp;custom_completion)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>arrive_and_wait_with_completion</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>a8c324216000deff76413134d9d20f2ac</anchor>
      <arglist>(uint32_t thread_id, C &amp;&amp;custom_completion)</arglist>
    </member>
    <member kind="function">
      <type>arrival_token_typed&lt; T &gt;</type>
      <name>arrive_reduce</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>a490cae3d5765604a127f119ca8984c44</anchor>
      <arglist>(uint32_t thread_id, T x, F reduce)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>wait_reduce</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>a3ab486e926ed61a093a52f07b4b9e059</anchor>
      <arglist>(arrival_token_typed&lt; T &gt; &amp;&amp;token)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>reduce</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>a8e5647fb88ae223437ad82fac5042d49</anchor>
      <arglist>(uint32_t thread_id, T x, F reduce)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>broadcast</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>ae15228d22aea106aa918a8c68700b845</anchor>
      <arglist>(uint32_t thread_id, T &amp;&amp;x, uint32_t src=0)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static constexpr uint32_t</type>
      <name>max</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>a028c34ec619414c0ff074e7f7367d6be</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>spin_count</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>a8e4d5d2a5e3235978ea3a4b661e96949</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef" protection="private">
      <type>typename State::ticket_t::value_type</type>
      <name>ticket_value_type</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>addb39c372a0c03009ac0616313d910f0</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" protection="private">
      <type>State::ticket_t &amp;</type>
      <name>get_local_phase</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>acc984d93658146f49529146d2d2234f0</anchor>
      <arglist>(uint32_t thread_id) noexcept</arglist>
    </member>
    <member kind="function" protection="private">
      <type>State::ticket_t &amp;</type>
      <name>get_local_line</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>a2e139954998ab1f002e259038a56dedf</anchor>
      <arglist>(uint32_t thread_id) noexcept</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>sanity_check_arrival</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>aa427fe1c89ea92b0858cbf520e010a97</anchor>
      <arglist>(uint32_t thread_id, BarrierPhase cur_phase) noexcept</arglist>
    </member>
    <member kind="function" protection="private">
      <type>bool</type>
      <name>arrive_impl</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>ae26c82825d976e99584e6ffbc987ba09</anchor>
      <arglist>(BarrierPhase old_phase, uint32_t thread_id)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>bool</type>
      <name>arrive_impl</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>a2eb476deaed0b2e74c1e0a7945cbc323</anchor>
      <arglist>(BarrierPhase old_phase, uint32_t thread_id, T value, F reduce)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>arrival_token</type>
      <name>arrive_with_completion</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>a96560e3d829ee7e3e274bb73ce6b1e1a</anchor>
      <arglist>(uint32_t thread_id, A arrival, C &amp;&amp;custom_completion)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>uint32_t</type>
      <name>expected</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>a034f2edaf5394e71e1d72a4b762846a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::unique_ptr&lt; State[]&gt;</type>
      <name>state</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>a2bc1e0767efb7d3fb220f09bb7b3873e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::unique_ptr&lt; Storage[]&gt;</type>
      <name>storage</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>aeb50ca8dad5468c919ffb1678ab686ba</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>Storage</type>
      <name>broadcast_storage</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>adb1514abba71637d7606bc7d3a2bb584</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>CompletionFn</type>
      <name>completion</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>a4e8072d66efe7d0bc170060e6cd69e91</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::atomic&lt; BarrierPhase &gt;</type>
      <name>phase</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>acab69cd401008a32fef359eeccae1afb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private" static="yes">
      <type>static constexpr size_t</type>
      <name>cache_line_size</name>
      <anchorfile>classcyqlone_1_1TreeBarrier.html</anchorfile>
      <anchor>a389929b1d19da95bbd820e0c62cbfa5c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>cyqlone::TreeBarrier::arrival_token</name>
    <filename>classcyqlone_1_1TreeBarrier_1_1arrival__token.html</filename>
    <member kind="function">
      <type></type>
      <name>arrival_token</name>
      <anchorfile>classcyqlone_1_1TreeBarrier_1_1arrival__token.html</anchorfile>
      <anchor>a3394d5ba5322a4b4c404431909003744</anchor>
      <arglist>(BarrierPhase phase)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>arrival_token</name>
      <anchorfile>classcyqlone_1_1TreeBarrier_1_1arrival__token.html</anchorfile>
      <anchor>adf8f6e6b6bd0134aa00b454551aa765b</anchor>
      <arglist>(const arrival_token &amp;phase)=delete</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>arrival_token</name>
      <anchorfile>classcyqlone_1_1TreeBarrier_1_1arrival__token.html</anchorfile>
      <anchor>ab9e0379e6ef5823055270291c7048003</anchor>
      <arglist>(arrival_token &amp;&amp;phase)=default</arglist>
    </member>
    <member kind="function">
      <type>arrival_token &amp;</type>
      <name>operator=</name>
      <anchorfile>classcyqlone_1_1TreeBarrier_1_1arrival__token.html</anchorfile>
      <anchor>a8e42ec36b4dd179183cd81c5c4ee28a3</anchor>
      <arglist>(const arrival_token &amp;phase)=delete</arglist>
    </member>
    <member kind="function">
      <type>arrival_token &amp;</type>
      <name>operator=</name>
      <anchorfile>classcyqlone_1_1TreeBarrier_1_1arrival__token.html</anchorfile>
      <anchor>a1e221f566edb39bfb99c1339225621e1</anchor>
      <arglist>(arrival_token &amp;&amp;phase)=default</arglist>
    </member>
    <member kind="function">
      <type>BarrierPhase</type>
      <name>get</name>
      <anchorfile>classcyqlone_1_1TreeBarrier_1_1arrival__token.html</anchorfile>
      <anchor>a0ae3adfb1b3a1eb629c33f0cb5ad2bb0</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>BarrierPhase</type>
      <name>phase</name>
      <anchorfile>classcyqlone_1_1TreeBarrier_1_1arrival__token.html</anchorfile>
      <anchor>a0f94a00070058f607d27b677b1183d48</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>cyqlone::TreeBarrier::arrival_token_typed</name>
    <filename>classcyqlone_1_1TreeBarrier_1_1arrival__token__typed.html</filename>
    <templarg>class T</templarg>
    <base>cyqlone::TreeBarrier&lt; CompletionFn, PhaseType &gt;::arrival_token</base>
    <member kind="function">
      <type>BarrierPhase</type>
      <name>get</name>
      <anchorfile>classcyqlone_1_1TreeBarrier_1_1arrival__token.html</anchorfile>
      <anchor>a0ae3adfb1b3a1eb629c33f0cb5ad2bb0</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>BarrierPhase</type>
      <name>phase</name>
      <anchorfile>classcyqlone_1_1TreeBarrier_1_1arrival__token.html</anchorfile>
      <anchor>a0f94a00070058f607d27b677b1183d48</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::TreeBarrier::State</name>
    <filename>structcyqlone_1_1TreeBarrier_1_1State.html</filename>
    <member kind="typedef">
      <type>std::atomic&lt; unsigned char &gt;</type>
      <name>atomic_byte</name>
      <anchorfile>structcyqlone_1_1TreeBarrier_1_1State.html</anchorfile>
      <anchor>a4125d55e90315f39314cfe52fd2b49a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>std::atomic&lt; uint32_t &gt;</type>
      <name>atomic_word</name>
      <anchorfile>structcyqlone_1_1TreeBarrier_1_1State.html</anchorfile>
      <anchor>a125c9cb8ce01a4c1dda0ca11c0516170</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>std::conditional_t&lt; only_word_lock_free, atomic_word, atomic_byte &gt;</type>
      <name>ticket_t</name>
      <anchorfile>structcyqlone_1_1TreeBarrier_1_1State.html</anchorfile>
      <anchor>aaeef4b36ea8581ca353d75bfa8a59bba</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::array&lt; ticket_t, num_levels &gt;</type>
      <name>tickets</name>
      <anchorfile>structcyqlone_1_1TreeBarrier_1_1State.html</anchorfile>
      <anchor>a386ec88e0ec815fe9fb73f49b27f2eba</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static constexpr bool</type>
      <name>only_word_lock_free</name>
      <anchorfile>structcyqlone_1_1TreeBarrier_1_1State.html</anchorfile>
      <anchor>acf06ece412b111b3f52daf9066523229</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static constexpr size_t</type>
      <name>num_levels</name>
      <anchorfile>structcyqlone_1_1TreeBarrier_1_1State.html</anchorfile>
      <anchor>a90b7ea856efdc3965247ddcfcea7ce92</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::TreeBarrier::Storage</name>
    <filename>structcyqlone_1_1TreeBarrier_1_1Storage.html</filename>
    <member kind="function">
      <type>void</type>
      <name>store</name>
      <anchorfile>structcyqlone_1_1TreeBarrier_1_1Storage.html</anchorfile>
      <anchor>a5ce2b2060ea157d53021884d41689cc2</anchor>
      <arglist>(T t) noexcept</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>load</name>
      <anchorfile>structcyqlone_1_1TreeBarrier_1_1Storage.html</anchorfile>
      <anchor>a6de786f0f1f7f30901b89c2fd05cbd92</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="variable">
      <type>std::array&lt; std::byte, cache_line_size &gt;</type>
      <name>payload</name>
      <anchorfile>structcyqlone_1_1TreeBarrier_1_1Storage.html</anchorfile>
      <anchor>aedb627fbe200d130180ea9e4526b3b64</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static constexpr bool</type>
      <name>is_compatible</name>
      <anchorfile>structcyqlone_1_1TreeBarrier_1_1Storage.html</anchorfile>
      <anchor>a432288f246cd6783ff7ffe3ae626e04d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::TricyqleParams</name>
    <filename>group__topic-block-tridiag-solvers.html</filename>
    <anchor>structcyqlone_1_1TricyqleParams</anchor>
    <templarg>class T</templarg>
    <member kind="typedef">
      <type>T</type>
      <name>value_type</name>
      <anchorfile>group__topic-block-tridiag-solvers.html</anchorfile>
      <anchor>a1109b77e541118bc80ced3d3abd9a4c2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>enable_prefetching</name>
      <anchorfile>group__topic-block-tridiag-solvers.html</anchorfile>
      <anchor>a3d92b585f8ec55a836c01da26d0d6fe7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>pcg_max_iter</name>
      <anchorfile>group__topic-block-tridiag-solvers.html</anchorfile>
      <anchor>ae371a93ef1548774fe9521c69cad3ff8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>value_type</type>
      <name>pcg_tolerance</name>
      <anchorfile>group__topic-block-tridiag-solvers.html</anchorfile>
      <anchor>a9b29bcd17341e5a316ea61abd6b34e03</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>pcg_print_resid</name>
      <anchorfile>group__topic-block-tridiag-solvers.html</anchorfile>
      <anchor>a4f6ed4a429d5e696aa6652b4c92644e7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>SolveMethod</type>
      <name>solve_method</name>
      <anchorfile>group__topic-block-tridiag-solvers.html</anchorfile>
      <anchor>abf65052fa3d46aa4fee1f881aa739cf5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>pcr_max_update_fraction</name>
      <anchorfile>group__topic-block-tridiag-solvers.html</anchorfile>
      <anchor>ad1ab0da44d008c8c43005f295943e64c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>cr_max_update_fraction_Y0</name>
      <anchorfile>group__topic-block-tridiag-solvers.html</anchorfile>
      <anchor>a06e0173d6207da3a1e86d313b670841c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>parallel_solve_cr_threshold</name>
      <anchorfile>group__topic-block-tridiag-solvers.html</anchorfile>
      <anchor>a8b4ea43ef95ad75544213330c2ceb549</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>parallel_factor_pcr_threshold</name>
      <anchorfile>group__topic-block-tridiag-solvers.html</anchorfile>
      <anchor>aa3014457155b9bcb1e0ead17f91d3a7d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cyqlone::TricyqleSolver</name>
    <filename>structcyqlone_1_1TricyqleSolver.html</filename>
    <templarg>index_t VL</templarg>
    <templarg>class T</templarg>
    <templarg>StorageOrder DefaultOrder</templarg>
    <templarg>class Ctx</templarg>
    <member kind="typedef">
      <type>batmat::datapar::deduced_simd&lt; value_type, v &gt;</type>
      <name>simd</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a6e2316e70914b3c4dfd6acd8fff51507</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>std::integral_constant&lt; index_t, v &gt;</type>
      <name>vl_t</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ac55d6eb560a7dc93f15007b973a02666</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>std::integral_constant&lt; index_t, v *alignof(value_type)&gt;</type>
      <name>align_t</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ab32be23ec2955b71575054732d86bf12</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const index_t</type>
      <name>p</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ae7132e8f61a245a3d01fc383af0eb89b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static constexpr index_t</type>
      <name>v</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ac6ba1b63d73d96fdb1d2c451f169198a</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>constexpr index_t</type>
      <name>lp</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a012802e173a8dc5b1a52ab48d6fbdbc6</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>constexpr index_t</type>
      <name>ceil_p</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ab930ba7a502f7aa12b5f52232e6dd011</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>constexpr index_t</type>
      <name>ceil_P</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a8b30dc4929eb30f8048490c6f2e00e2e</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>std::unique_ptr&lt; SharedContext &gt;</type>
      <name>create_parallel_context</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ab98b5d86fc2f72471977777817e14477</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static constexpr index_t</type>
      <name>lv</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a1b890c32ae59332f69161cbbe617b9b5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="typedef">
      <type>batmat::matrix::Matrix&lt; value_type, index_t, vl_t, index_t, O, align_t &gt;</type>
      <name>matrix</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>af2d639079f2a4177927e21ba1bc185e3</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>batmat::matrix::View&lt; const value_type, index_t, vl_t, index_t, index_t, O &gt;</type>
      <name>view</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a73b19d75b029ae42618090a2a2c3ee05</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>batmat::matrix::View&lt; value_type, index_t, vl_t, index_t, index_t, O &gt;</type>
      <name>mut_view</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a75b2a12d350e9576136a01c7a3ab7256</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>batmat::matrix::DefaultStride</type>
      <name>layer_stride</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a68624e7bd45dd1c8aaa6edf7d04f2b8c</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>batmat::matrix::View&lt; const value_type, index_t, vl_t, vl_t, layer_stride, O &gt;</type>
      <name>batch_view</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>abdab27c99dc16a17c69ea6fb3783ce80</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>batmat::matrix::View&lt; value_type, index_t, vl_t, vl_t, layer_stride, O &gt;</type>
      <name>mut_batch_view</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a213872b5c37a2175d0525b84d7746c2a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static constexpr auto</type>
      <name>default_order</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ae6f4b4f37bc5824bb1ccb7ed84fb6dcb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static constexpr auto</type>
      <name>column_major</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a581dd3048d84810abaeea53a0c795165</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const index_t</type>
      <name>block_size</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ad75b0e935664202d452a2b78cb850fa4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const index_t</type>
      <name>max_rank</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a271ee9921d01590c9c6c807d1489f875</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>circular</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a8acb277cac93b1388932b018ac8a8759</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Params</type>
      <name>params</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ad9e7de9d9bfbef18a82cfb9a27bf9f73</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>Params</type>
      <name>get_params</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ab80914e2ee1ef4e656941d5ca9b7c92e</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update_params</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ae93193e17c2e8ef8eb74570242fcca43</anchor>
      <arglist>(const Params &amp;new_params)</arglist>
    </member>
    <member kind="variable">
      <type>matrix&lt; default_order &gt;</type>
      <name>cr_L</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>aef557f2fcdecb0dcff9d05ca4f41a70d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix&lt; default_order &gt;</type>
      <name>cr_U</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>aed8696a9b34627dbdb69128ce0f1542c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix&lt; default_order &gt;</type>
      <name>cr_Y</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>aa1507aaa86895db2fd1e012a7fd45493</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix&lt; column_major &gt;</type>
      <name>work_cr</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>aab3fa77b85fe119473eb797a74d4d869</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix&lt; default_order &gt;</type>
      <name>pcr_L</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a997da195519a15cf284223f486421cff</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix&lt; default_order &gt;</type>
      <name>pcr_Y</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>aee76308f1db45678cb49400f5fbe20d4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix&lt; default_order &gt;</type>
      <name>pcr_U</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a8d54a1b6808c4c6b02100237d4e15326</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix&lt; default_order &gt;</type>
      <name>pcr_M</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ac11c0ec3fd165450dd92fb58cdda74bc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix&lt; column_major &gt;</type>
      <name>work_pcg</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ad29ac663646e5eaab3f30de99e6b7225</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; index_t &gt;</type>
      <name>m_update</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>aa350fb6608fc7f9e8535310491b35c9c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>m_update_u0</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ad183a7591435762193d7262b1f541533</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix&lt; column_major &gt;</type>
      <name>work_update_Σ</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ac43adfa5658483c1ef92129a913fe61d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix&lt; column_major &gt;</type>
      <name>work_update</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a9dcc302bc8ed48fb5afb19bebc356ec1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix&lt; column_major &gt;</type>
      <name>work_hyh</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ad6c7118afcfbb945e241928d3204fb30</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix&lt; column_major &gt;</type>
      <name>work_update_pcr_Σ</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>af1cbe1aa32b82909ad0c9b5bc4dc0ea8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix&lt; column_major &gt;</type>
      <name>work_update_pcr_L</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a67fc9cc4fde97fab289e5f31df38fd20</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrix&lt; column_major &gt;</type>
      <name>work_update_pcr_UY</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a5764772ea2258319d42b2ee2fb13a8cd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static constexpr bool</type>
      <name>merge_last_level_pcr</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a8731c7fb3868ef8a43a28dde8d7c20c0</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>factor_pcr</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ab77d3511e2d8d513f8c6bf225bbd1148</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>factor_pcr_level</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>afc99f3e0c97a8abd568553ca869b81d1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>factor_pcr_parallel</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>af4e4da068b7b88c23bec221ca7cb19e4</anchor>
      <arglist>(Context &amp;ctx)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>factor_pcr_level_parallel</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ad7503136804e8ef1863af78ca16256cf</anchor>
      <arglist>(Context &amp;ctx)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve_pcr</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ae788d7e0258330b80bc6f145bd2e782f</anchor>
      <arglist>(mut_batch_view&lt;&gt; λ, mut_batch_view&lt;&gt; work_pcr) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve_pcr</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a6446d45f3c1de75ce5028dcffb6adc16</anchor>
      <arglist>(mut_batch_view&lt;&gt; λ)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve_pcr_level</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>aeb42515d69d37fefba719d63bce4fcc6</anchor>
      <arglist>(mut_batch_view&lt;&gt; λ, mut_batch_view&lt;&gt; work_pcr) const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>ν2</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a5b53d7cd56d117046b6f7bab08f36e68</anchor>
      <arglist>(index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>ν2p</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a782a7ace2e04f33aaf3a8f13a4232ba2</anchor>
      <arglist>(index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>add_wrap_ceil_p</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a78b124800e3965de03b989e01f873fe7</anchor>
      <arglist>(index_t a, index_t b) const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>sub_wrap_ceil_p</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a86afffc20cf6af66e8138d44b0369048</anchor>
      <arglist>(index_t a, index_t b) const</arglist>
    </member>
    <member kind="function">
      <type>decltype(auto)</type>
      <name>init_diag</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a7eecdb1909dd1a65cb78834c79199b67</anchor>
      <arglist>(Context &amp;ctx, auto &amp;&amp;func)</arglist>
    </member>
    <member kind="function">
      <type>decltype(auto)</type>
      <name>init_subdiag</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a3671e0a9d5c2aa2f516177cbf1c03a1d</anchor>
      <arglist>(Context &amp;ctx, auto &amp;&amp;func)</arglist>
    </member>
    <member kind="function">
      <type>decltype(auto)</type>
      <name>init_rhs</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ae351b004148b4c11a514b8fe67deab06</anchor>
      <arglist>(Context &amp;ctx, mut_view&lt;&gt; b, auto &amp;&amp;func) const</arglist>
    </member>
    <member kind="function">
      <type>decltype(auto)</type>
      <name>get_solution</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>aaa4c6e267c9e96900bf137636527826d</anchor>
      <arglist>(Context &amp;ctx, view&lt;&gt; λ, auto &amp;&amp;func) const</arglist>
    </member>
    <member kind="function">
      <type>decltype(auto)</type>
      <name>get_solution</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a2d9dc77d4b9112cf99f4157bcc28d086</anchor>
      <arglist>(Context &amp;ctx, mut_view&lt;&gt; λ, auto &amp;&amp;func) const</arglist>
    </member>
    <member kind="function">
      <type>decltype(auto)</type>
      <name>get_solution</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a76b8e42ec50126a2e76abb2ff9ed787d</anchor>
      <arglist>(Context &amp;ctx, Λ &amp;&amp;λ, F &amp;&amp;func) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>factor_solve</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>abb66b9ab00524f3a7c4f97bb613c6f4e</anchor>
      <arglist>(Context &amp;ctx, mut_view&lt;&gt; λ, index_t stride=1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>factor</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>aca744d77be095bd640bfb8a7caef7acb</anchor>
      <arglist>(Context &amp;ctx)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve_forward</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a85512f82f61415003df4a4e0493a5c87</anchor>
      <arglist>(Context &amp;ctx, mut_view&lt;&gt; λ, index_t stride=1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve_reverse</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ad596a6fbbcfc164ff47712ef6979a47f</anchor>
      <arglist>(Context &amp;ctx, mut_view&lt;&gt; λ, mut_view&lt;&gt; work, index_t stride=1) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve_reverse</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a7acf517e211c6af865514470f7a0f97c</anchor>
      <arglist>(Context &amp;ctx, mut_view&lt;&gt; λ, index_t stride=1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>factor_solve_skip_first</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a457309aeb8df1ca12b924167665dd89b</anchor>
      <arglist>(Context &amp;ctx, mut_view&lt;&gt; λ, index_t stride=1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>factor_skip_first</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a4454ac2f1dc5c556ddf5a7820b4f416f</anchor>
      <arglist>(Context &amp;ctx)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve_forward_skip_first</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a945d837a9d6703fc6e04c50dfaddce98</anchor>
      <arglist>(Context &amp;ctx, mut_view&lt;&gt; λ, index_t stride=1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>factor_solve_impl</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a079ac7c6ff83c4f86f9d6f3e36b0323e</anchor>
      <arglist>(Context &amp;ctx, mut_view&lt;&gt; λ, index_t stride=1)</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>cr_thread_assignment</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a88657d8567c66e6c943d38e438d79777</anchor>
      <arglist>(index_t l, index_t c) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>factor_U</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>abbe1c2dcb3b966fc084268ec1676678e</anchor>
      <arglist>(index_t l, index_t iU)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>factor_Y</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>afe6a4a4ca91d69efb43ade0ba4054841</anchor>
      <arglist>(index_t l, index_t iY)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>factor_L</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a2c0ffa018a7c2bb9d7ac6cfefbb5f98d</anchor>
      <arglist>(index_t l, index_t i)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update_K</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ae97f7140e58b083c1a36c7243e818a90</anchor>
      <arglist>(index_t l, index_t i)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve_u_forward</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ae4591af87afa3b1be848242d55255727</anchor>
      <arglist>(index_t l, index_t iU, mut_view&lt;&gt; λ, index_t stride) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve_y_forward</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a6e6fa4c03aaa50dfc12d6cdd07e9bdd7</anchor>
      <arglist>(index_t l, index_t iY, mut_view&lt;&gt; λ, mut_view&lt;&gt; w, index_t stride) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve_λ_forward</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>aae62156d7bfb43d0d1e2007502945106</anchor>
      <arglist>(index_t l, index_t iL, mut_view&lt;&gt; λ, view&lt;&gt; w, index_t stride) const</arglist>
    </member>
    <member kind="function">
      <type>value_type</type>
      <name>mul_Mv</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>af6ad00ee9b4799413bb51953dc32e635</anchor>
      <arglist>(batch_view&lt;&gt; p, mut_batch_view&lt;&gt; Mp, batch_view&lt; default_order &gt; L, batch_view&lt; default_order &gt; K) const</arglist>
    </member>
    <member kind="function">
      <type>value_type</type>
      <name>mul_precond</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a0fbd5c0b3944ba370d1f448b7c089a4a</anchor>
      <arglist>(batch_view&lt;&gt; r, mut_batch_view&lt;&gt; z, mut_batch_view&lt;&gt; w, batch_view&lt; default_order &gt; L, batch_view&lt; default_order &gt; K) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve_pcg</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ac51461b5f6e6a0b6cf1fceee77849e80</anchor>
      <arglist>(mut_batch_view&lt;&gt; λ, mut_batch_view&lt;&gt; work_pcg) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve_pcg</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>abf875ae26597e548b375e0447ae35f81</anchor>
      <arglist>(mut_batch_view&lt;&gt; λ)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve_reverse_parallel</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ad044086888337234913060153bbee989</anchor>
      <arglist>(Context &amp;ctx, mut_view&lt;&gt; λ, mut_view&lt;&gt; work, index_t stride) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve_reverse_serial</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a10c86c70d917a48ad3e4370bf280d80f</anchor>
      <arglist>(mut_view&lt;&gt; λ, mut_view&lt;&gt; work, index_t stride) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve_u_backward</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a71d6ec2361b8e139cf680f6148624031</anchor>
      <arglist>(index_t l, index_t iU, mut_view&lt;&gt; λ, mut_view&lt;&gt; w, index_t stride) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve_y_backward</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ab0251f6b1c1171da318153cbe010a173</anchor>
      <arglist>(index_t l, index_t iY, mut_view&lt;&gt; λ, index_t stride) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>solve_λ_backward</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>aa7ba2344be32f08b28d8e42997d2c727</anchor>
      <arglist>(index_t biL, mut_view&lt;&gt; λ, view&lt;&gt; w, index_t stride) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_thread_update_rank</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ab2d088fd97e377a761ca36bc18e7e731</anchor>
      <arglist>(Context &amp;ctx, index_t c, index_t m)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_update_rank_extra</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>aadc2486fed8dd6aeced7c25dd03ce3d7</anchor>
      <arglist>(index_t m)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clear_update_rank_extra</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>afbeba2d23106fcdd6f701072a0d80568</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::pair&lt; index_t, index_t &gt;</type>
      <name>cols_Ups_fwd</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a9a70221a5683b2f47fc92ab4b091381f</anchor>
      <arglist>(index_t l, index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>std::pair&lt; index_t, index_t &gt;</type>
      <name>cols_Ups_bwd</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>aecf8b11cbd6a78b76ef930c9dd02c23b</anchor>
      <arglist>(index_t l, index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>std::pair&lt; index_t, index_t &gt;</type>
      <name>cols_Q_cr</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>aa3b840d3945940d297e9454fc45471c9</anchor>
      <arglist>(index_t l, index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>work_Ups_fwd_w</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ae96bad78f07d455c03e2a4c57f3f20dd</anchor>
      <arglist>(index_t l, index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>index_t</type>
      <name>work_Ups_bwd_w</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>aad99bc44d7521300bbf748fc35197160</anchor>
      <arglist>(index_t l, index_t i) const</arglist>
    </member>
    <member kind="function">
      <type>mut_batch_view&lt; column_major &gt;</type>
      <name>work_Ups_fwd</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>aaf978742dce0716381c21c706735efe4</anchor>
      <arglist>(index_t l, index_t i)</arglist>
    </member>
    <member kind="function">
      <type>mut_batch_view&lt; column_major &gt;</type>
      <name>work_Ups_bwd</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a8e401f18ecb9b49d7787acda0c4b82bb</anchor>
      <arglist>(index_t l, index_t i)</arglist>
    </member>
    <member kind="function">
      <type>mut_batch_view&lt; column_major &gt;</type>
      <name>work_Q_cr</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a6c548372032957ad3d6bb45b969cee1c</anchor>
      <arglist>(index_t l, index_t i)</arglist>
    </member>
    <member kind="function">
      <type>mut_batch_view&lt; column_major &gt;</type>
      <name>work_Σ_fwd</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a21e24f961831d5d817ecaf035a388a53</anchor>
      <arglist>(index_t l, index_t i)</arglist>
    </member>
    <member kind="function">
      <type>mut_batch_view&lt; column_major &gt;</type>
      <name>work_Σ_bwd</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a85bbeefbbe578eadb6789edab65b8afe</anchor>
      <arglist>(index_t l, index_t i)</arglist>
    </member>
    <member kind="function">
      <type>mut_batch_view&lt; column_major &gt;</type>
      <name>work_Σ_Q</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a3a45a2ca31d7043d314ef4a5c9028d07</anchor>
      <arglist>(index_t l, index_t i)</arglist>
    </member>
    <member kind="function">
      <type>mut_batch_view&lt; column_major &gt;</type>
      <name>work_Ups_fwd_last</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a305ed5c288d9f40e1cc7a624211fc5eb</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>mut_batch_view&lt; column_major &gt;</type>
      <name>work_Ups_bwd_last</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>aac41b3a925ebaac6387c33c297143039</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>mut_batch_view&lt; column_major &gt;</type>
      <name>work_Σ_fwd_last</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>aad0af6d5d47c436c5c810244f7404299</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>mut_batch_view&lt; column_major &gt;</type>
      <name>work_Σ_bwd_last</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a31a3e83f62ba97d20feacebf52246333</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>mut_batch_view&lt; column_major &gt;</type>
      <name>work_Ups_extra</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a3ae75a128c0d3fdd721518b3569c4f48</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>mut_batch_view&lt; column_major &gt;</type>
      <name>work_Σ_extra</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a14941a03d5d61a4be68db99c26c22234</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update_solve_cr</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ab24424cbf36846c6a6bd7e1bca4740d0</anchor>
      <arglist>(Context &amp;ctx, mut_view&lt;&gt; λ, index_t stride)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update_L</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a9cc9b6b3c1711803ca7105a75ac5f0cb</anchor>
      <arglist>(index_t l, index_t i)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update_U</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>af3ddcc1bfa5a4a52ef865f36b50d6d87</anchor>
      <arglist>(index_t l, index_t i)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update_Y</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>aebda48fc697452e5b8e28e4b1d0240b4</anchor>
      <arglist>(index_t l, index_t i)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update_pcr_level</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>aa6c3b62e6f8d321f0579a0bb32666e8f</anchor>
      <arglist>(index_t m, mut_batch_view&lt;&gt; WYU, mut_batch_view&lt;&gt; WΣ)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update_pcr</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a006f5da4ecca9bb027562621b10b19af</anchor>
      <arglist>(batch_view&lt;&gt; fwd, batch_view&lt;&gt; bwd, batch_view&lt;&gt; Σ)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>prefetch</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ad61264e04e719bc54739e6b01b3c723c</anchor>
      <arglist>(batch_view&lt; O &gt; X) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>prefetch_L</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a2c3ed7c3c1d0cb46274ca9f201980c7a</anchor>
      <arglist>(batch_view&lt; O &gt; X) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>prefetch_L</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a45bf2441956c8207e6f5023ce094d652</anchor>
      <arglist>(index_t bi) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>prefetch_U</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>ad3f339007ff3e53daae64ef955b65239</anchor>
      <arglist>(index_t l, index_t iU) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>prefetch_Y</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>af6ead2b15d84f82cf4559eedb477eb1c</anchor>
      <arglist>(index_t l, index_t iY) const</arglist>
    </member>
    <member kind="typedef">
      <type>T</type>
      <name>value_type</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a34cb4d531e90a26bcafc87246efe5afd</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>TricyqleParams&lt; value_type &gt;</type>
      <name>Params</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>af08ff8665d6d36c51901dc9f56ecebc7</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>Ctx</type>
      <name>Context</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>affb1753c99cde7e3136139a84f8fbb54</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename Context::shared_context_type</type>
      <name>SharedContext</name>
      <anchorfile>structcyqlone_1_1TricyqleSolver.html</anchorfile>
      <anchor>a50d42179c3fec57d48737271045bc203</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>Options</name>
    <filename>benchmarks_2cyqpalm_2spring-mass_8cpp.html</filename>
    <anchor>structOptions</anchor>
    <member kind="variable">
      <type>bool</type>
      <name>cold</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a3de04e8f7bbb951b210863e1714ed62a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>warm_shift</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>af35db881cccf91a2eded77afa50b70fb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>warm_copy</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a33a25a66e106fd98e8eb8e22a2ad6952</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>no_updates</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a15b9dc7e39dbeb050697735dbe1929e6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; int &gt;</type>
      <name>parallelism</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a84366d66b118e7062a152985348239cd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; int &gt;</type>
      <name>vector_length</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a7c864f62e787e28ea788125a0b5b59c1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>rm</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a028a843a11fc7b7ed2654be9b8c82a42</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>cm</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>aa87c76cef3252d6411bce3893b442e5c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>pcr</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>ae4d3df5515efa3730d7789445f52fd97</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>hpipm</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a5fb516d0404207b4a43f572c2df77f10</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; int &gt;</type>
      <name>horizon</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>add5121947cc0caf0f36002d9139b42d7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; int &gt;</type>
      <name>masses</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a6e54944195345a63ad09c39e0bf0e073</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint64_t</type>
      <name>num_instances</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a28f0b7978ade7a3be01900e7ce473280</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint64_t</type>
      <name>seed</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>ad0cd58ab8b36006862e57094b9be42b5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>ProblemType</type>
      <name>problem_type</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a30184c9dceaa1948064492129b48dea0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>pcr_max_update_fraction</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a8090adb2740c14588adaf6a54259fcc2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>cr_max_update_fraction</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a6dcb695bee4b0a8bd43c71a59af746f0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>parallel_solve_cr_threshold</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a9ed388d2e25d400369ad3c4dbee92ed0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>parallel_factor_pcr_threshold</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>aae3b8bfee6ceac8cd95a967c33cad452</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>changing_constr_factor</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a6016e9e0cd591f88d4b471f602a1bab7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>custom_reporter</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a7127e3b6cdb1170a62208979c97c32aa</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>print_extra</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>ae279fc65b1d933a072077a630cbc9e3a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>use_color</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a0c157ff2f12fcd0535ad0d57680dc155</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>trace</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a2078f04e236cb4491abc5d00d863ed81</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::string</type>
      <name>export_problem</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>af57c4c88687fda5d62a62e8b0dc4008d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>Problem</name>
    <filename>benchmarks_2cyqpalm_2spring-mass_8cpp.html</filename>
    <anchor>structProblem</anchor>
    <member kind="variable">
      <type>std::string</type>
      <name>name</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a2086d27dcbd2dc82aa31c44869aada91</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>SpringMassParams</type>
      <name>params</name>
      <anchorfile>benchmarks_2cyqpalm_2spring-mass_8cpp.html</anchorfile>
      <anchor>a56720c210bc176b2068647ae4795b4f9</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>Solver</name>
    <filename>structSolver.html</filename>
    <member kind="variable">
      <type>std::string</type>
      <name>name</name>
      <anchorfile>structSolver.html</anchorfile>
      <anchor>aa3cc0c1424c904084583322bc3a14f5f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void(benchmark::State &amp;, SpringMassParams)&gt;</type>
      <name>run</name>
      <anchorfile>structSolver.html</anchorfile>
      <anchor>aae9371b91862765a1a99f1287ed4cdf7</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>SpringMassParams</name>
    <filename>structSpringMassParams.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>ActuatorPlacement</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>a18d1032c9ae1e2b5bbe9df30c50413f1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IndividualActuators</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a18d1032c9ae1e2b5bbe9df30c50413f1ac68a5dc9e1917b565380d1ab6e675849</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>RandomActuators</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a18d1032c9ae1e2b5bbe9df30c50413f1a30d42f40d22544857de9a609caf07ae2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>RandomPairsOfActuators</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a18d1032c9ae1e2b5bbe9df30c50413f1a617eba72fd8a383f9f58874e67c9e912</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>WangBoydActuators</name>
      <anchorfile>structcyqlone_1_1qpalm_1_1problems_1_1SpringMassParams.html</anchorfile>
      <anchor>a18d1032c9ae1e2b5bbe9df30c50413f1a9ed41af8dca64bf2fe5d84464f710060</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IndividualActuators</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>a18d1032c9ae1e2b5bbe9df30c50413f1ac68a5dc9e1917b565380d1ab6e675849</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>RandomActuators</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>a18d1032c9ae1e2b5bbe9df30c50413f1a30d42f40d22544857de9a609caf07ae2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>RandomPairsOfActuators</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>a18d1032c9ae1e2b5bbe9df30c50413f1a617eba72fd8a383f9f58874e67c9e912</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>WangBoydActuators</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>a18d1032c9ae1e2b5bbe9df30c50413f1a9ed41af8dca64bf2fe5d84464f710060</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>static SpringMassParams</type>
      <name>wang_boyd_2008</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>ac8f9cdf3a81a4bd2ff3b8f01143035af</anchor>
      <arglist>(index_t n_masses, index_t N_horiz=30, uint64_t seed=0)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static SpringMassParams</type>
      <name>wang_boyd_2008_width</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>a2c47e72483a5afacffa1fb7449ede6b5</anchor>
      <arglist>(index_t n_masses, index_t N_horiz=30, uint64_t seed=0, double steady_state_spring_length=0.1)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static SpringMassParams</type>
      <name>domahidi_2012</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>ac85087e18caea8b4d4b755e273ae740d</anchor>
      <arglist>(index_t n_masses, index_t N_horiz, uint64_t seed=0)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static SpringMassParams</type>
      <name>active_state_constr</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>abc65cdb2b4298fb3a0324b5aeaa81e8f</anchor>
      <arglist>(index_t n_masses=18, index_t N_horiz=256, uint64_t seed=0)</arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>friction</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>a1dba013a3d54b283f9a36f97bd5f1f2e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>k_spring</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>ab5a2af96e4f38127c9e16a4581b86679</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>F_max</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>a522c2f182381139c34e576be1652e0d9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>p_max</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>ae51c660af8c130707f451dde118e4a3c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>p_min</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>a610a7a0c64707d230b7f120570aaf41f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>p_min_f</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>a70e147415444754a12f9d828c7a2d6d6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>p_max_f</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>a20401c1d408994186ca5f1316b346f40</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>v_max</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>a6a7aac80e23a91f8e1861dec691d5a03</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>v_max_f</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>a6a89e4327b4809fdf0b0e61f111c2dfe</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>width</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>a887fc5679d33abe3be614792cddffd85</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>N_horiz</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>af6edc6dec7fa7b2a9e15766d9748f9b4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>T_horiz</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>a202950abd5806db1f1dec39c2bb3d51b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>q_vel</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>a9fbc86d261ebb7bdb01a7f37a58d0174</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>q_pos</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>af8a2fd9a992b8305847a25fd24c9d1bd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>q_vel_f</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>a5443953a7bc84041776f986b5fe0e519</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>q_pos_f</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>ac567b9cf366b0a724de6ac966e915e89</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>real_t</type>
      <name>r_act</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>a1ba21c87e90b5089efb04b787d5b4da9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; real_t &gt;</type>
      <name>masses</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>a0a29b4bcbbbcb45b1874cd956a811071</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>index_t</type>
      <name>n_actuators</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>acfc2b8ef98e88e9b219e1eedd13f4b7b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>enum cyqlone::qpalm::problems::SpringMassParams::ActuatorPlacement</type>
      <name>actuator_placement</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>ac172425aa0e56c2a6db220e71e700b3e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint64_t</type>
      <name>seed</name>
      <anchorfile>structSpringMassParams.html</anchorfile>
      <anchor>a6c6b090dc45b6c7e373a408fbefdbe54</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>TridiagSystem</name>
    <filename>solve-block-tridiagonal_8cpp.html</filename>
    <anchor>structTridiagSystem</anchor>
    <member kind="variable">
      <type>matrices</type>
      <name>M</name>
      <anchorfile>solve-block-tridiagonal_8cpp.html</anchorfile>
      <anchor>abc697170b9533d791d25d633fb52b837</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrices</type>
      <name>K</name>
      <anchorfile>solve-block-tridiagonal_8cpp.html</anchorfile>
      <anchor>a83a123410ddf401f3cab7e507d8eb50d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>matrices</type>
      <name>b</name>
      <anchorfile>solve-block-tridiagonal_8cpp.html</anchorfile>
      <anchor>aa5af71777abff45ccfd6f6a4c715f1d1</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>cyqlone</name>
    <filename>namespacecyqlone.html</filename>
    <namespace>cyqlone::detail</namespace>
    <namespace>cyqlone::linalg</namespace>
    <namespace>cyqlone::parallel</namespace>
    <namespace>cyqlone::qpalm</namespace>
    <class kind="struct">cyqlone::EmptyCompletion</class>
    <class kind="class">cyqlone::TreeBarrier</class>
    <class kind="struct">cyqlone::LinearOCPSparseQP</class>
    <class kind="struct">cyqlone::TricyqleParams</class>
    <class kind="struct">cyqlone::CyqloneParams</class>
    <class kind="struct">cyqlone::CyqloneStorage</class>
    <class kind="struct">cyqlone::TricyqleSolver</class>
    <class kind="struct">cyqlone::CyqloneSolver</class>
    <class kind="class">cyqlone::NeumaierSum</class>
    <class kind="struct">cyqlone::OCPDim</class>
    <class kind="struct">cyqlone::LinearOCPStorage</class>
    <class kind="struct">cyqlone::norms</class>
    <class kind="struct">cyqlone::norms&lt; T, void &gt;</class>
    <class kind="struct">cyqlone::SparseMatrix</class>
    <class kind="struct">cyqlone::SparseMatrixBuilder</class>
    <class kind="struct">cyqlone::matio_traits</class>
    <class kind="struct">cyqlone::matio_traits&lt; float &gt;</class>
    <class kind="struct">cyqlone::matio_traits&lt; double &gt;</class>
    <class kind="struct">cyqlone::matio_traits&lt; I &gt;</class>
    <class kind="struct">cyqlone::PCRFactorTest</class>
    <class kind="struct">cyqlone::SparseCSC</class>
    <class kind="struct">cyqlone::SparseCOO</class>
    <member kind="typedef">
      <type>::_mat_t</type>
      <name>mat_t</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gac7f041cab5ae3c1471243d06da6f6469</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>std::unique_ptr&lt; mat_t, int(*)(mat_t *)&gt;</type>
      <name>MatFilePtr</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gaceac871d1342196be7468d2bb3fcd386</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>batmat::DefaultTimings</type>
      <name>DefaultTimings</name>
      <anchorfile>namespacecyqlone.html</anchorfile>
      <anchor>ab50cdc97a7f508bd11dbdeac98e83024</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>std::unique_ptr&lt; matvar_t, decltype(&amp;Mat_VarFree)&gt;</type>
      <name>MatVarPtr</name>
      <anchorfile>namespacecyqlone.html</anchorfile>
      <anchor>a93737f44f96b7895aa6d2ba4874d66b1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>SolveMethod</name>
      <anchorfile>group__topic-block-tridiag-solvers.html</anchorfile>
      <anchor>ga922388e973fa665d5f5eb0c7d988c81f</anchor>
      <arglist></arglist>
      <enumvalue file="group__topic-block-tridiag-solvers.html" anchor="gga922388e973fa665d5f5eb0c7d988c81fa91fa024e7c3efdd56efd4588853cf488">StairPCG</enumvalue>
      <enumvalue file="group__topic-block-tridiag-solvers.html" anchor="gga922388e973fa665d5f5eb0c7d988c81fa811f52a4a3648918ef143a0cb6a6e334">JacobiPCG</enumvalue>
      <enumvalue file="group__topic-block-tridiag-solvers.html" anchor="gga922388e973fa665d5f5eb0c7d988c81fac0ff1599838f84547b6668a463198617">PCR</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>MatioOpenMode</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga8e268a5d303978ce26bb366dd8db62e4</anchor>
      <arglist></arglist>
      <enumvalue file="group__topic-util-matio.html" anchor="gga8e268a5d303978ce26bb366dd8db62e4a7a1a5f3e79fdc91edf2f5ead9d66abb4">Read</enumvalue>
      <enumvalue file="group__topic-util-matio.html" anchor="gga8e268a5d303978ce26bb366dd8db62e4a1129c0e4d43f2d121652a7302712cff6">Write</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>StorageOrder</name>
      <anchorfile>namespacecyqlone.html</anchorfile>
      <anchor>a98ef36a3bcaa63feb09e9a30189c18fe</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>Symmetry</name>
      <anchorfile>namespacecyqlone.html</anchorfile>
      <anchor>a397fa707bcca52f40de82f40667e8382</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reference_to_gradient</name>
      <anchorfile>group__topic-ocp-formulations.html</anchorfile>
      <anchor>ga61795701135500f6aaf49cc698e5976e</anchor>
      <arglist>(const LinearOCPStorage &amp;ocp, std::span&lt; const real_t &gt; ref, std::span&lt; real_t &gt; qr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reference_to_gradient</name>
      <anchorfile>namespacecyqlone.html</anchorfile>
      <anchor>a551aa703936f7c27064994d2e8353d07</anchor>
      <arglist>(LinearOCPStorage &amp;ocp, std::span&lt; const real_t &gt; ref)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>enum_name</name>
      <anchorfile>namespacecyqlone.html</anchorfile>
      <anchor>a8a31b253ce198a9b911790669ab1f91a</anchor>
      <arglist>(SolveMethod s)</arglist>
    </member>
    <member kind="function">
      <type>constexpr bool</type>
      <name>is_pow_2</name>
      <anchorfile>namespacecyqlone.html</anchorfile>
      <anchor>a8de86ba12c6b45e9164ba9c951a8eb78</anchor>
      <arglist>(index_t n)</arglist>
    </member>
    <member kind="function">
      <type>constexpr index_t</type>
      <name>ceil_log2</name>
      <anchorfile>namespacecyqlone.html</anchorfile>
      <anchor>a65a236b4a58a2be6cc7768e05a583145</anchor>
      <arglist>(index_t n)</arglist>
    </member>
    <member kind="function">
      <type>constexpr index_t</type>
      <name>get_level</name>
      <anchorfile>namespacecyqlone.html</anchorfile>
      <anchor>a028e7b47650ae9e52eeded0d2cff39a3</anchor>
      <arglist>(index_t i)</arglist>
    </member>
    <member kind="function">
      <type>constexpr index_t</type>
      <name>get_index_in_level</name>
      <anchorfile>namespacecyqlone.html</anchorfile>
      <anchor>a3e39a85e130b8ecc38f9f61cdcfcb0c1</anchor>
      <arglist>(index_t i)</arglist>
    </member>
    <member kind="function">
      <type>MatFilePtr</type>
      <name>open_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga9a8d93b3cfc96c16fa9c70b969388c50</anchor>
      <arglist>(const std::filesystem::path &amp;filename, MatioOpenMode mode=MatioOpenMode::Read)</arglist>
    </member>
    <member kind="function">
      <type>MatFilePtr</type>
      <name>create_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga5b9bfdc6a8d4f53838511888e6724cb8</anchor>
      <arglist>(const std::filesystem::path &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga937f13ae7ced1e23973c106d256995a0</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, float value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga42854ade7f449f8478e5e3015d800a30</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, double value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga98ba9fcc8ae30f52267a6ed391911aa5</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, short value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga318cacc90af5fd74e3949f88a3a0c7e3</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, int value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga3dd29ca4a01d166d48251c7a5ea8ce34</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, long value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gaea09b40b9de36a89d1c91e595170c7c5</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, long long value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga60956f86298592566c35039d92a5edc3</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, unsigned short value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gafecae51b359b38c163783aba289d7218</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, unsigned int value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga1537cff5aed44fd0e58c4fee61675161</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, unsigned long value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga07ddf7772fe145d98e1ebdf2d9677cc1</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, unsigned long long value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gaaf4aac1a36819fc07e647af186b19e76</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, guanaqo::MatrixView&lt; const double, index_t &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gad4f62a7347cceafe8574c7e741f991cf</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, guanaqo::MatrixView&lt; const float, index_t &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga7f4118c8f4b3b9fd0dccfc578989e2b6</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, batmat::matrix::View&lt; const double, index_t &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gadc1a730925581865e91ec60b5c2d1cc0</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, batmat::matrix::View&lt; const float, index_t &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga81a3620a6c3dabf0e6bfd8f56960c300</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::span&lt; const float &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga581b8a2280d7eff0b756086643618db8</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::span&lt; const double &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga8ce5583b813db30a7ff21fb2568a3b3b</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::span&lt; const unsigned short &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga2a4285ac5b76e2829f06405eeaa7f484</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::span&lt; const unsigned int &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gadeddae83552dc401d00f90d86df1e610</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::span&lt; const unsigned long &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gac2fda9834d3303e31d9fb598ef949809</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::span&lt; const unsigned long long &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gaeac3d6a0248c962a3d5b29a91a0d08d0</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::span&lt; const short &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gaa20a79941500e76a4777644e83df8aa0</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::span&lt; const int &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga9007be7514cd0af283eb395d3396acc2</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::span&lt; const long &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gafb115ff7b2443481e6480f95caa3aaa0</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::span&lt; const long long &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gae79d8af02b2c40cfa0b8edd01ff7fa94</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, const SparseMatrix &amp;matrix)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga043f9e47579e7bf1529ad7baa0c86a12</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, const LinearOCPStorage &amp;ocp)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>read_from_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga9811702bc5a357d1064eb05bff17d4b6</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, LinearOCPStorage &amp;ocp)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>read_from_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga8400d97982852012a2f51f369a0da870</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::vector&lt; float &gt; &amp;data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>read_from_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga6f27d652223be08a963b98316c4b7b45</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::vector&lt; double &gt; &amp;data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>read_from_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga6d5e9459f93f03c070fed01a07cb66ba</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::span&lt; float &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>read_from_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga710eb1987fa4f8b1fda296b05fdfc685</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::span&lt; double &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>ocp_dump_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gaf1ab96eebb1218ba8573c6d61ec461bf</anchor>
      <arglist>(const std::filesystem::path &amp;filename, const LinearOCPStorage &amp;ocp)</arglist>
    </member>
    <member kind="function">
      <type>LinearOCPStorage</type>
      <name>generate_random_ocp</name>
      <anchorfile>group__topic-ocp-formulations.html</anchorfile>
      <anchor>gafc61f634e110905af0dffeb3e27a4fd8</anchor>
      <arglist>(OCPDim dim, uint_fast32_t seed=0)</arglist>
    </member>
    <member kind="function">
      <type>MatVarPtr</type>
      <name>create_tensor_var</name>
      <anchorfile>namespacecyqlone.html</anchorfile>
      <anchor>a7dd262e037bfffcdd646baa1182222af</anchor>
      <arglist>(const char *name, std::span&lt; const T &gt; buffer, std::array&lt; index_t, N &gt; dims)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write_tensor</name>
      <anchorfile>namespacecyqlone.html</anchorfile>
      <anchor>af0a5391e8b15b281535db221cbcc0ddd</anchor>
      <arglist>(mat_t *matfp, const char *name, std::span&lt; const T &gt; buffer, std::array&lt; index_t, N &gt; dims)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat_impl</name>
      <anchorfile>namespacecyqlone.html</anchorfile>
      <anchor>a8c332e0901eb510e4793c517343b6f58</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, guanaqo::MatrixView&lt; const T, index_t &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_to_mat_impl</name>
      <anchorfile>namespacecyqlone.html</anchorfile>
      <anchor>a27b68eeeac46e7c830d14cdd4e8c83d7</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, batmat::matrix::View&lt; const T, index_t &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>validate_mat_var</name>
      <anchorfile>namespacecyqlone.html</anchorfile>
      <anchor>a5ee0a358b384d4ffadc58319d367a3c7</anchor>
      <arglist>(const matvar_t *var, const std::string &amp;name, int expected_rank)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>open_vector_var</name>
      <anchorfile>namespacecyqlone.html</anchorfile>
      <anchor>a62f2112e792f3d024ec5d651ed6fd94a</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname)</arglist>
    </member>
    <member kind="function">
      <type>constexpr index_t</type>
      <name>get_depth</name>
      <anchorfile>namespacecyqlone.html</anchorfile>
      <anchor>a33fa61ac58e944169799f69b27212f92</anchor>
      <arglist>(index_t n)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>λ_max_power</name>
      <anchorfile>namespacecyqlone.html</anchorfile>
      <anchor>a00902118a90d14866e828aefd56f4bd3</anchor>
      <arglist>(const Mat &amp;M, const Mat &amp;K, int max_it, typename Mat::value_type tol)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>unpacked</name>
      <anchorfile>namespacecyqlone.html</anchorfile>
      <anchor>aa29ae73cccae530f445e9ba571b6d949</anchor>
      <arglist>(const M &amp;matrix)</arglist>
    </member>
    <member kind="function">
      <type>constexpr auto</type>
      <name>simdify</name>
      <anchorfile>namespacecyqlone.html</anchorfile>
      <anchor>a83105b13bb65f22f974c2faffffd40fe</anchor>
      <arglist>(simdifiable auto &amp;&amp;a) -&gt; simdified_view_t&lt; decltype(a)&gt;</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>cyqlone::detail</name>
    <filename>namespacecyqlone_1_1detail.html</filename>
    <member kind="function">
      <type>void</type>
      <name>copy</name>
      <anchorfile>namespacecyqlone_1_1detail.html</anchorfile>
      <anchor>a3cbfac4ccd22478ebb900ef8a1a80c8f</anchor>
      <arglist>(guanaqo::MatrixView&lt; T1, I1, S1, O1 &gt; src, guanaqo::MatrixView&lt; T2, I2, S2, O2 &gt; dst)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scale</name>
      <anchorfile>namespacecyqlone_1_1detail.html</anchorfile>
      <anchor>a9ac31d8e0699acdf2a99776e66251afe</anchor>
      <arglist>(T0 scalar, guanaqo::MatrixView&lt; T1, I1, S1, O1 &gt; src, guanaqo::MatrixView&lt; T2, I2, S2, O2 &gt; dst)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>cyqlone::linalg</name>
    <filename>namespacecyqlone_1_1linalg.html</filename>
    <namespace>cyqlone::linalg::multi</namespace>
    <member kind="function">
      <type>norms&lt; simdified_value_t&lt; Vx &gt; &gt;::result</type>
      <name>norms_all</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga5b021d3e61d99e49576a574e7c919923</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>norm_inf</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga2332e39f282f0b22d60d5e0a57f4e932</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>norm_1</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga972ef7eb9a7e6a2c3680dfb95737e083</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>norm_2_squared</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga60918683b4f31e9f30b1b40fb12fdc97</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>norm_2</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gae11e8e17f4b74608291fe5b2d389869c</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>dot</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gac25c43a1c180bb6ad0faedbde44a20a9</anchor>
      <arglist>(Vx &amp;&amp;x, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scale</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaeee444a2231976efc5f93a58cada45c5</anchor>
      <arglist>(T alpha, Vx &amp;&amp;x, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scale</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga16a11c4519ab1e0cf4946c57bf4d0671</anchor>
      <arglist>(T alpha, Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>hadamard</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gae8ee607462b95f6106dc714e1fe86f60</anchor>
      <arglist>(Vx &amp;&amp;x, Vy &amp;&amp;y, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>hadamard</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga5f7dead7502413b423294c480fe67df7</anchor>
      <arglist>(Vx &amp;&amp;x, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clamp</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gacd0f6a1148300c8895b047baafd80619</anchor>
      <arglist>(Vx &amp;&amp;x, Vlo &amp;&amp;lo, Vhi &amp;&amp;hi, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clamp_resid</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga7c05b45f1fc5c69ed29a8cb7f0d4cd33</anchor>
      <arglist>(Vx &amp;&amp;x, Vlo &amp;&amp;lo, Vhi &amp;&amp;hi, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>axpby</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga7798d7d6e1ab0065fd4abc60fcc2e437</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Tb beta, Vy &amp;&amp;y, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>axpby</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga7be0c0501654d83ed8cc615b5d19d4e9</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Tb beta, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>axpy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga3f4f3b9f53f734e9a29368d36a5d80a0</anchor>
      <arglist>(Vy &amp;&amp;y, const std::array&lt; simdified_value_t&lt; Vy &gt;, sizeof...(Vx)&gt; &amp;alphas, Vx &amp;&amp;...x)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>axpy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaf92a45bb3dfc15de8dcd96ee9bd2345b</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Vy &amp;&amp;y, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>axpy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga6a42b0d6640680263a1c97afd7a6e2cb</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>negate</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga8107de760e8af9ee6dddc4f9878c1339</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, with_rotate_t&lt; Rotate &gt;={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>negate</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaae5b326a925859f5c11cddbd75b1b057</anchor>
      <arglist>(VA &amp;&amp;A, with_rotate_t&lt; Rotate &gt;={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sub</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gafb26638c219b8754db25e88f69304439</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, VC &amp;&amp;C, with_rotate_t&lt; Rotate &gt;={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sub</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga907ac42d74d99fe2a6abc9c4fcd80b2b</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, with_rotate_t&lt; Rotate &gt;={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga63e5d7edfbf2831545aea00d9a75966b</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, VC &amp;&amp;C, with_rotate_t&lt; Rotate &gt;={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gab6af7e9086aefaf875651e751e1ad92b</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, with_rotate_t&lt; Rotate &gt;={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>for_each_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga42110f9f447dd30af3fdc936a07d1a39</anchor>
      <arglist>(F &amp;&amp;fun, VA &amp;&amp;A, VAs &amp;&amp;...As)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>transform_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga5b35f1651072845d48695ccefa5ef549</anchor>
      <arglist>(F &amp;&amp;fun, VA &amp;&amp;A, VAs &amp;&amp;...As)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>transform2_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gacaef92140287b1c3e3c41c2a71ee69ab</anchor>
      <arglist>(F &amp;&amp;fun, VA &amp;&amp;A, VB &amp;&amp;B, VAs &amp;&amp;...As)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>transform_n_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaaa852f170922addceaa82a4cb63349c6</anchor>
      <arglist>(F &amp;&amp;fun, std::tuple&lt; VAs... &gt; As, VBs &amp;&amp;...Bs)</arglist>
    </member>
    <member kind="function">
      <type>norms&lt; simdified_value_t&lt; Vx &gt; &gt;::result</type>
      <name>norms_all</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gade4eb9023d1b94d7657f68dc256860b8</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>norm_inf</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga55c086fc7235a79e26c3aa5b1154c224</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>norm_1</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gabf875f22442b0023c2142b00d0975554</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>norm_2_squared</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gae934751791c316edde36013271945bf3</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>norm_2</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga91b1ca93bfba644b53749eeac8edb444</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>dot</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gafac39321d4eca4894c8099e597a9b5f2</anchor>
      <arglist>(Vx &amp;&amp;x, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scale</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gad40329bb0eea1a12dd5b510935b93fca</anchor>
      <arglist>(T alpha, Vx &amp;&amp;x, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scale</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga4dd6dc0ecf27af5d2f74f12e0aaca26a</anchor>
      <arglist>(T alpha, Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>hadamard</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga192f6d5da228f92a9e16216064fdac0e</anchor>
      <arglist>(Vx &amp;&amp;x, Vy &amp;&amp;y, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>hadamard</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga970509394d0f01a224acb3eac0f97535</anchor>
      <arglist>(Vx &amp;&amp;x, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clamp</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaaea202adb9d942b00a786f902e45307d</anchor>
      <arglist>(Vx &amp;&amp;x, Vlo &amp;&amp;lo, Vhi &amp;&amp;hi, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clamp_resid</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga60b2783e234cd9c038d1067b4f529a38</anchor>
      <arglist>(Vx &amp;&amp;x, Vlo &amp;&amp;lo, Vhi &amp;&amp;hi, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>axpby</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga2c0d5fb8f9ea81725e5d54d7605129e3</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Tb beta, Vy &amp;&amp;y, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>axpby</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaca21e3c51e47359f16410a524f1a6896</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Tb beta, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>axpy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaede77b28f5e46753ee7b915935f144b5</anchor>
      <arglist>(Vy &amp;&amp;y, const std::array&lt; simdified_value_t&lt; Vy &gt;, sizeof...(Vx)&gt; &amp;alphas, Vx &amp;&amp;...x)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>axpy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga30451a0e4f4100069ff65963d1bb0dbf</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Vy &amp;&amp;y, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>axpy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga74efe6fd79de11b4fad66ce0444c2de3</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>negate</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga4024401b27cb17a5982d63dca91df61b</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, with_rotate_t&lt; Rotate &gt; rot={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>negate</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gadfa94d9946d30ead1c8a0896599bd313</anchor>
      <arglist>(VA &amp;&amp;A, with_rotate_t&lt; Rotate &gt; rot={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sub</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaff7f734df0a2b041994f963e2226f858</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, VC &amp;&amp;C, with_rotate_t&lt; Rotate &gt; rot={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sub</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga48d07be903d60f6d48b7c3af0e9da414</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, with_rotate_t&lt; Rotate &gt; rot={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gac534f2ffcc1821dedd98afd67b4d5e27</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, VC &amp;&amp;C, with_rotate_t&lt; Rotate &gt; rot={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga84c12f9e93355fcc36107e18086ef7a4</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, with_rotate_t&lt; Rotate &gt; rot={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>for_each_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaa72458de29526c1deb6a0519926b2cd0</anchor>
      <arglist>(F &amp;&amp;fun, VA &amp;&amp;A, VAs &amp;&amp;...As)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>transform_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga04fa9fa6e82563be75524b11f135bc2b</anchor>
      <arglist>(F &amp;&amp;fun, VA &amp;&amp;A, VAs &amp;&amp;...As)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>transform2_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga79cf0bb8d436a1cba7ce70fab44bfbf0</anchor>
      <arglist>(F &amp;&amp;fun, VA &amp;&amp;A, VB &amp;&amp;B, VAs &amp;&amp;...As)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>transform_n_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga52c5beff69e8bdfb213cd47caacc104f</anchor>
      <arglist>(F &amp;&amp;fun, std::tuple&lt; VAs... &gt; As, VBs &amp;&amp;...Bs)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>copy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga85860ca1111492777572a2ba091cf5ca</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, Opts... opts)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>copy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga2e50e5f96654eb673d29ac8461143e91</anchor>
      <arglist>(Structured&lt; VA, S &gt; A, Structured&lt; VB, S &gt; B, Opts... opts)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>unpack</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga526900bf6322e7c42895c80c36ac2f5e</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>pack</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gae29d94393be39ebcd6ce5f909e28787c</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>cyqlone::linalg::multi</name>
    <filename>namespacecyqlone_1_1linalg_1_1multi.html</filename>
    <member kind="function">
      <type>norms&lt; simdified_value_t&lt; Vx &gt; &gt;::result</type>
      <name>norms_all</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gade4eb9023d1b94d7657f68dc256860b8</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>norm_inf</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga55c086fc7235a79e26c3aa5b1154c224</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>norm_1</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gabf875f22442b0023c2142b00d0975554</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>norm_2_squared</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gae934751791c316edde36013271945bf3</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>norm_2</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga91b1ca93bfba644b53749eeac8edb444</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>dot</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gafac39321d4eca4894c8099e597a9b5f2</anchor>
      <arglist>(Vx &amp;&amp;x, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scale</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gad40329bb0eea1a12dd5b510935b93fca</anchor>
      <arglist>(T alpha, Vx &amp;&amp;x, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scale</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga4dd6dc0ecf27af5d2f74f12e0aaca26a</anchor>
      <arglist>(T alpha, Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>hadamard</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga192f6d5da228f92a9e16216064fdac0e</anchor>
      <arglist>(Vx &amp;&amp;x, Vy &amp;&amp;y, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>hadamard</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga970509394d0f01a224acb3eac0f97535</anchor>
      <arglist>(Vx &amp;&amp;x, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clamp</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaaea202adb9d942b00a786f902e45307d</anchor>
      <arglist>(Vx &amp;&amp;x, Vlo &amp;&amp;lo, Vhi &amp;&amp;hi, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clamp_resid</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga60b2783e234cd9c038d1067b4f529a38</anchor>
      <arglist>(Vx &amp;&amp;x, Vlo &amp;&amp;lo, Vhi &amp;&amp;hi, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>axpby</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga2c0d5fb8f9ea81725e5d54d7605129e3</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Tb beta, Vy &amp;&amp;y, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>axpby</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaca21e3c51e47359f16410a524f1a6896</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Tb beta, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>axpy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaede77b28f5e46753ee7b915935f144b5</anchor>
      <arglist>(Vy &amp;&amp;y, const std::array&lt; simdified_value_t&lt; Vy &gt;, sizeof...(Vx)&gt; &amp;alphas, Vx &amp;&amp;...x)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>axpy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga30451a0e4f4100069ff65963d1bb0dbf</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Vy &amp;&amp;y, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>axpy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga74efe6fd79de11b4fad66ce0444c2de3</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>negate</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga4024401b27cb17a5982d63dca91df61b</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, with_rotate_t&lt; Rotate &gt; rot={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>negate</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gadfa94d9946d30ead1c8a0896599bd313</anchor>
      <arglist>(VA &amp;&amp;A, with_rotate_t&lt; Rotate &gt; rot={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sub</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaff7f734df0a2b041994f963e2226f858</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, VC &amp;&amp;C, with_rotate_t&lt; Rotate &gt; rot={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sub</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga48d07be903d60f6d48b7c3af0e9da414</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, with_rotate_t&lt; Rotate &gt; rot={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gac534f2ffcc1821dedd98afd67b4d5e27</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, VC &amp;&amp;C, with_rotate_t&lt; Rotate &gt; rot={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga84c12f9e93355fcc36107e18086ef7a4</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, with_rotate_t&lt; Rotate &gt; rot={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>for_each_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaa72458de29526c1deb6a0519926b2cd0</anchor>
      <arglist>(F &amp;&amp;fun, VA &amp;&amp;A, VAs &amp;&amp;...As)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>transform_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga04fa9fa6e82563be75524b11f135bc2b</anchor>
      <arglist>(F &amp;&amp;fun, VA &amp;&amp;A, VAs &amp;&amp;...As)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>transform2_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga79cf0bb8d436a1cba7ce70fab44bfbf0</anchor>
      <arglist>(F &amp;&amp;fun, VA &amp;&amp;A, VB &amp;&amp;B, VAs &amp;&amp;...As)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>transform_n_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga52c5beff69e8bdfb213cd47caacc104f</anchor>
      <arglist>(F &amp;&amp;fun, std::tuple&lt; VAs... &gt; As, VBs &amp;&amp;...Bs)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>copy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga85860ca1111492777572a2ba091cf5ca</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, Opts... opts)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>copy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga2e50e5f96654eb673d29ac8461143e91</anchor>
      <arglist>(Structured&lt; VA, S &gt; A, Structured&lt; VB, S &gt; B, Opts... opts)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>cyqlone::parallel</name>
    <filename>namespacecyqlone_1_1parallel.html</filename>
    <class kind="struct">cyqlone::parallel::Context</class>
    <class kind="struct">cyqlone::parallel::SharedContext</class>
  </compound>
  <compound kind="namespace">
    <name>cyqlone::qpalm</name>
    <filename>namespacecyqlone_1_1qpalm.html</filename>
    <namespace>cyqlone::qpalm::problems</namespace>
    <namespace>cyqlone::qpalm::detail</namespace>
    <class kind="struct">cyqlone::qpalm::CyqloneData</class>
    <class kind="struct">cyqlone::qpalm::CyQPALMBackendSettings</class>
    <class kind="struct">cyqlone::qpalm::CyQPALMBackendStats</class>
    <class kind="struct">cyqlone::qpalm::CyQPALMBackend</class>
    <class kind="struct">cyqlone::qpalm::unique_CyQPALMBackend</class>
    <class kind="struct">cyqlone::qpalm::DetailedStats</class>
    <class kind="struct">cyqlone::qpalm::Breakpoint</class>
    <class kind="struct">cyqlone::qpalm::ABSums</class>
    <class kind="struct">cyqlone::qpalm::PartitionedBreakpoints</class>
    <class kind="struct">cyqlone::qpalm::BreakpointsResult</class>
    <class kind="struct">cyqlone::qpalm::compute_breakpoints_fn</class>
    <class kind="struct">cyqlone::qpalm::get_partitioned_breakpoints_fn</class>
    <class kind="struct">cyqlone::qpalm::get_breakpoints_fn</class>
    <class kind="struct">cyqlone::qpalm::LineSearchSettings</class>
    <class kind="struct">cyqlone::qpalm::LineSearch</class>
    <class kind="struct">cyqlone::qpalm::SolverImplementation</class>
    <class kind="struct">cyqlone::qpalm::Settings</class>
    <class kind="struct">cyqlone::qpalm::SolverTimings</class>
    <class kind="struct">cyqlone::qpalm::SolverStats</class>
    <class kind="class">cyqlone::qpalm::Solver</class>
    <member kind="typedef">
      <type>real_t</type>
      <name>ABSum_t</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a584914f2e3545fba9420d6b1c5c09059</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>WarmStartingStrategy</name>
      <anchorfile>group__topic-optimization-solvers-ocp.html</anchorfile>
      <anchor>gaeb4dd1f31c1ec8d3d9e0f9023da5cec3</anchor>
      <arglist></arglist>
      <enumvalue file="group__topic-optimization-solvers-ocp.html" anchor="ggaeb4dd1f31c1ec8d3d9e0f9023da5cec3a8b7a61a6e9bd5b8977b7b3f02a425adc">Zeros</enumvalue>
      <enumvalue file="group__topic-optimization-solvers-ocp.html" anchor="ggaeb4dd1f31c1ec8d3d9e0f9023da5cec3a5fb63579fc981698f97d55bfecb213ea">Copy</enumvalue>
      <enumvalue file="group__topic-optimization-solvers-ocp.html" anchor="ggaeb4dd1f31c1ec8d3d9e0f9023da5cec3a825a3d98017bab11815ad2817201324c">Shift</enumvalue>
      <enumvalue file="group__topic-optimization-solvers-ocp.html" anchor="ggaeb4dd1f31c1ec8d3d9e0f9023da5cec3ab882fa82c60e5b9a229e5ed6e52ad31a">ShiftNoInequality</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>SolverStatus</name>
      <anchorfile>group__topic-optimization-solvers.html</anchorfile>
      <anchor>gade0917661457f4dc0d6396ded24277a4</anchor>
      <arglist></arglist>
      <enumvalue file="group__topic-optimization-solvers.html" anchor="ggade0917661457f4dc0d6396ded24277a4ad8a942ef2b04672adfafef0ad817a407">Busy</enumvalue>
      <enumvalue file="group__topic-optimization-solvers.html" anchor="ggade0917661457f4dc0d6396ded24277a4ada5418b1eb4e80d05f2c1c7c4fa1b6ec">Converged</enumvalue>
      <enumvalue file="group__topic-optimization-solvers.html" anchor="ggade0917661457f4dc0d6396ded24277a4a9e8b9350c8f1716701ec1f9bbe76432e">MaxTime</enumvalue>
      <enumvalue file="group__topic-optimization-solvers.html" anchor="ggade0917661457f4dc0d6396ded24277a4abbf52264f7a6e91c48a242f95aeed3db">MaxIter</enumvalue>
      <enumvalue file="group__topic-optimization-solvers.html" anchor="ggade0917661457f4dc0d6396ded24277a4aff6e97b110349dae9c6e8be0ff7a85c3">NotFinite</enumvalue>
      <enumvalue file="group__topic-optimization-solvers.html" anchor="ggade0917661457f4dc0d6396ded24277a4ab644b4f0de4cf0c46acb4583602e168c">NoProgress</enumvalue>
      <enumvalue file="group__topic-optimization-solvers.html" anchor="ggade0917661457f4dc0d6396ded24277a4a12c37c4278b8c0db6c2f65052569cd80">Interrupted</enumvalue>
      <enumvalue file="group__topic-optimization-solvers.html" anchor="ggade0917661457f4dc0d6396ded24277a4ab0d4998a26f5b5742ad38c4af8817e32">Exception</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>StorageOrder</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a12561d95ffd5ca9cc074d7b3edceb429</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>unique_CyQPALMBackend&lt; VL, DefaultOrder &gt;</type>
      <name>make_cyqpalm_backend</name>
      <anchorfile>group__topic-optimization-solvers-ocp.html</anchorfile>
      <anchor>gae148b32eb392bb39d685929069cfabdb</anchor>
      <arglist>(const CyqloneStorage&lt; real_t &gt; &amp;ocp, CyqloneData data, const CyQPALMBackendSettings &amp;settings)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update_cyqpalm_backend</name>
      <anchorfile>group__topic-optimization-solvers-ocp.html</anchorfile>
      <anchor>gaf5dbe2437a5f7176b58b7743cba83d1c</anchor>
      <arglist>(CyQPALMBackend&lt; VL, DefaultOrder &gt; &amp;backend, const CyqloneStorage&lt; real_t &gt; &amp;ocp)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update_cyqpalm_backend</name>
      <anchorfile>group__topic-optimization-solvers-ocp.html</anchorfile>
      <anchor>ga8aac8ebf6f85aba82b32a927bc9bea00</anchor>
      <arglist>(CyQPALMBackend&lt; VL, DefaultOrder &gt; &amp;backend, const LinearOCPStorage &amp;ocp)</arglist>
    </member>
    <member kind="function">
      <type>unique_CyQPALMBackend&lt; VL, DefaultOrder &gt;</type>
      <name>make_cyqpalm_backend</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a0d06e7a2fb9288cf51ce720d27a14baf</anchor>
      <arglist>(const CyqloneStorage&lt;&gt; &amp;ocp, CyqloneData data, const CyQPALMBackendSettings &amp;settings)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>sort</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>aaa9bd6f0faae51b2d93379a451da5206</anchor>
      <arglist>(R &amp;&amp;range, F key)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>nth_element</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>af0491991e6f9a32072489c45d934541b</anchor>
      <arglist>(R &amp;&amp;range, I mid, F key)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static std::ranges::subrange&lt; std::ranges::iterator_t&lt; R &gt; &gt;</type>
      <name>partition_min</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a907fb321c6e4e7fa1375233be5c9f815</anchor>
      <arglist>(R &amp;&amp;range, F pred, C cmp)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static decltype(auto)</type>
      <name>partition</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a1f120beae4aaa6b4c80f83809088d0e3</anchor>
      <arglist>(R &amp;&amp;range, F key)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static decltype(auto)</type>
      <name>partition</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a6446ea453ccfe8d17c950f1a9a348e12</anchor>
      <arglist>(I first, S last, F key)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static decltype(auto)</type>
      <name>min_element</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>acc34e5d9c2599830eb0f4f519dc2c791</anchor>
      <arglist>(R &amp;&amp;range, F key)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>transform_reduce</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a278e2594d3f1fc905558b7b5c8c1378e</anchor>
      <arglist>(I first, I last, T init, BinOp binary_op, UnOp unary_op)</arglist>
    </member>
    <member kind="function">
      <type>ABSums</type>
      <name>partial_sum_negative</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a3e3de90bb2d8bb912950e1aa6141b065</anchor>
      <arglist>(PartitionedBreakpoints breakpoints, real_t η=0, real_t β=0)</arglist>
    </member>
    <member kind="function">
      <type>std::span&lt; Breakpoint &gt;</type>
      <name>compute_breakpoints_default</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>ac2625e16a723c214f3e23b72f59ad817</anchor>
      <arglist>(std::vector&lt; Breakpoint &gt; &amp;breakpoints, const Vec &amp;Σ, const Vec &amp;y, const Vec &amp;Ad, const Vec &amp;Ax, const Vec &amp;b_min, const Vec &amp;b_max)</arglist>
    </member>
    <member kind="function">
      <type>PartitionedBreakpoints</type>
      <name>partition_breakpoints_default</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a57b32dc88357c5b2b160d9c09b949c93</anchor>
      <arglist>(std::span&lt; Breakpoint &gt; breakpoints)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>enum_name</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a58838550654934492da37499013b24d8</anchor>
      <arglist>(SolverStatus s)</arglist>
    </member>
    <member kind="function">
      <type>std::ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a8c85a6a89dd8e96977a86abad68b5d5c</anchor>
      <arglist>(std::ostream &amp;os, SolverStatus s)</arglist>
    </member>
    <member kind="function">
      <type>template unique_CyQPALMBackend&lt; 1, StorageOrder::ColMajor &gt;</type>
      <name>make_cyqpalm_backend&lt; 1, StorageOrder::ColMajor &gt;</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a2f204cd23b6824fa6bc96a5509d41edb</anchor>
      <arglist>(const CyqloneStorage&lt;&gt; &amp;ocp, CyqloneData data, const CyQPALMBackendSettings &amp;settings)</arglist>
    </member>
    <member kind="function">
      <type>template void</type>
      <name>update_cyqpalm_backend&lt; 1, StorageOrder::ColMajor &gt;</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>ab585f169928943f6459d5842bf956468</anchor>
      <arglist>(CyQPALMBackend&lt; 1, StorageOrder::ColMajor &gt; &amp;, const CyqloneStorage&lt;&gt; &amp;ocp)</arglist>
    </member>
    <member kind="function">
      <type>template void</type>
      <name>update_cyqpalm_backend&lt; 1, StorageOrder::ColMajor &gt;</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a1b42a175a558c7d15014e8d1e5f35e17</anchor>
      <arglist>(CyQPALMBackend&lt; 1, StorageOrder::ColMajor &gt; &amp;, const LinearOCPStorage &amp;ocp)</arglist>
    </member>
    <member kind="function">
      <type>template unique_CyQPALMBackend&lt; 4, StorageOrder::ColMajor &gt;</type>
      <name>make_cyqpalm_backend&lt; 4, StorageOrder::ColMajor &gt;</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a79f0ae995ed7eb26c1190599436819e1</anchor>
      <arglist>(const CyqloneStorage&lt;&gt; &amp;ocp, CyqloneData data, const CyQPALMBackendSettings &amp;settings)</arglist>
    </member>
    <member kind="function">
      <type>template void</type>
      <name>update_cyqpalm_backend&lt; 4, StorageOrder::ColMajor &gt;</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a97f95a80f835db53479b55aede5c5727</anchor>
      <arglist>(CyQPALMBackend&lt; 4, StorageOrder::ColMajor &gt; &amp;, const CyqloneStorage&lt;&gt; &amp;ocp)</arglist>
    </member>
    <member kind="function">
      <type>template void</type>
      <name>update_cyqpalm_backend&lt; 4, StorageOrder::ColMajor &gt;</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>aac8173f4f423dec08f6d05883a487f2c</anchor>
      <arglist>(CyQPALMBackend&lt; 4, StorageOrder::ColMajor &gt; &amp;, const LinearOCPStorage &amp;ocp)</arglist>
    </member>
    <member kind="function">
      <type>template unique_CyQPALMBackend&lt; 8, StorageOrder::ColMajor &gt;</type>
      <name>make_cyqpalm_backend&lt; 8, StorageOrder::ColMajor &gt;</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a934baf37e367a903f0011b30b8a4aed7</anchor>
      <arglist>(const CyqloneStorage&lt;&gt; &amp;ocp, CyqloneData data, const CyQPALMBackendSettings &amp;settings)</arglist>
    </member>
    <member kind="function">
      <type>template void</type>
      <name>update_cyqpalm_backend&lt; 8, StorageOrder::ColMajor &gt;</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a6c72550fee87ca239bd8cf6ebf328294</anchor>
      <arglist>(CyQPALMBackend&lt; 8, StorageOrder::ColMajor &gt; &amp;, const CyqloneStorage&lt;&gt; &amp;ocp)</arglist>
    </member>
    <member kind="function">
      <type>template void</type>
      <name>update_cyqpalm_backend&lt; 8, StorageOrder::ColMajor &gt;</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>af1728fda54e6061da9342445412b957d</anchor>
      <arglist>(CyQPALMBackend&lt; 8, StorageOrder::ColMajor &gt; &amp;, const LinearOCPStorage &amp;ocp)</arglist>
    </member>
    <member kind="function">
      <type>template unique_CyQPALMBackend&lt; 1, StorageOrder::RowMajor &gt;</type>
      <name>make_cyqpalm_backend&lt; 1, StorageOrder::RowMajor &gt;</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>ac583ca9eff1c425f194434aec7b48b63</anchor>
      <arglist>(const CyqloneStorage&lt;&gt; &amp;ocp, CyqloneData data, const CyQPALMBackendSettings &amp;settings)</arglist>
    </member>
    <member kind="function">
      <type>template void</type>
      <name>update_cyqpalm_backend&lt; 1, StorageOrder::RowMajor &gt;</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a3c2d9d42bb1ddc3f169cec3a93c28646</anchor>
      <arglist>(CyQPALMBackend&lt; 1, StorageOrder::RowMajor &gt; &amp;, const CyqloneStorage&lt;&gt; &amp;ocp)</arglist>
    </member>
    <member kind="function">
      <type>template void</type>
      <name>update_cyqpalm_backend&lt; 1, StorageOrder::RowMajor &gt;</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a01b08da46621a7c625ba8b4f3ee3021a</anchor>
      <arglist>(CyQPALMBackend&lt; 1, StorageOrder::RowMajor &gt; &amp;, const LinearOCPStorage &amp;ocp)</arglist>
    </member>
    <member kind="function">
      <type>template unique_CyQPALMBackend&lt; 4, StorageOrder::RowMajor &gt;</type>
      <name>make_cyqpalm_backend&lt; 4, StorageOrder::RowMajor &gt;</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>aa59386df69f60ced9a2dd111d5ebd6db</anchor>
      <arglist>(const CyqloneStorage&lt;&gt; &amp;ocp, CyqloneData data, const CyQPALMBackendSettings &amp;settings)</arglist>
    </member>
    <member kind="function">
      <type>template void</type>
      <name>update_cyqpalm_backend&lt; 4, StorageOrder::RowMajor &gt;</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a4fe47de5c740f85dfd2fe730ed892539</anchor>
      <arglist>(CyQPALMBackend&lt; 4, StorageOrder::RowMajor &gt; &amp;, const CyqloneStorage&lt;&gt; &amp;ocp)</arglist>
    </member>
    <member kind="function">
      <type>template void</type>
      <name>update_cyqpalm_backend&lt; 4, StorageOrder::RowMajor &gt;</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>abec2428948043461b0a8ad92e3bb3d44</anchor>
      <arglist>(CyQPALMBackend&lt; 4, StorageOrder::RowMajor &gt; &amp;, const LinearOCPStorage &amp;ocp)</arglist>
    </member>
    <member kind="function">
      <type>template unique_CyQPALMBackend&lt; 8, StorageOrder::RowMajor &gt;</type>
      <name>make_cyqpalm_backend&lt; 8, StorageOrder::RowMajor &gt;</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a1ca8f68ee1e0bc4e089eb3db751a2f6d</anchor>
      <arglist>(const CyqloneStorage&lt;&gt; &amp;ocp, CyqloneData data, const CyQPALMBackendSettings &amp;settings)</arglist>
    </member>
    <member kind="function">
      <type>template void</type>
      <name>update_cyqpalm_backend&lt; 8, StorageOrder::RowMajor &gt;</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a52066fb50c5a90391061dcc71365fafb</anchor>
      <arglist>(CyQPALMBackend&lt; 8, StorageOrder::RowMajor &gt; &amp;, const CyqloneStorage&lt;&gt; &amp;ocp)</arglist>
    </member>
    <member kind="function">
      <type>template void</type>
      <name>update_cyqpalm_backend&lt; 8, StorageOrder::RowMajor &gt;</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a37219f7b9f34c0ff4e574e598d2d308b</anchor>
      <arglist>(CyQPALMBackend&lt; 8, StorageOrder::RowMajor &gt; &amp;, const LinearOCPStorage &amp;ocp)</arglist>
    </member>
    <member kind="variable">
      <type>struct cyqlone::qpalm::compute_breakpoints_fn</type>
      <name>compute_breakpoints</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a7e1db9ae6e521ae2f4ce103e38f3f934</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct cyqlone::qpalm::get_partitioned_breakpoints_fn</type>
      <name>get_partitioned_breakpoints</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>a3908dbec9d527354fdfc9eebec4d8d30</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct cyqlone::qpalm::get_breakpoints_fn</type>
      <name>get_breakpoints</name>
      <anchorfile>namespacecyqlone_1_1qpalm.html</anchorfile>
      <anchor>acefd63912f9919fa5ab62ac0a305c3f2</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>cyqlone::qpalm::detail</name>
    <filename>namespacecyqlone_1_1qpalm_1_1detail.html</filename>
    <class kind="struct">cyqlone::qpalm::detail::backend_stats_type&lt; CyQPALMBackend&lt; VL, DefaultOrder &gt; &gt;</class>
    <class kind="struct">cyqlone::qpalm::detail::backend_type&lt; unique_CyQPALMBackend&lt; VL, DefaultOrder &gt; &gt;</class>
    <class kind="struct">cyqlone::qpalm::detail::backend_type</class>
    <class kind="struct">cyqlone::qpalm::detail::backend_type&lt; std::unique_ptr&lt; T, D &gt; &gt;</class>
    <class kind="struct">cyqlone::qpalm::detail::backend_type&lt; T * &gt;</class>
    <class kind="struct">cyqlone::qpalm::detail::backend_stats_type</class>
    <member kind="typedef">
      <type>typename backend_type&lt; T &gt;::type</type>
      <name>backend_type_t</name>
      <anchorfile>namespacecyqlone_1_1qpalm_1_1detail.html</anchorfile>
      <anchor>afc785dc408dd582906d25d8b9f7b5139</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>cyqlone::qpalm::problems</name>
    <filename>namespacecyqlone_1_1qpalm_1_1problems.html</filename>
    <class kind="struct">cyqlone::qpalm::problems::PlatooningParams</class>
    <class kind="struct">cyqlone::qpalm::problems::PlatooningProblem</class>
    <class kind="struct">cyqlone::qpalm::problems::SpringMassParams</class>
    <class kind="struct">cyqlone::qpalm::problems::SpringMassProblem</class>
    <member kind="typedef">
      <type>Eigen::MatrixX&lt; real_t &gt;</type>
      <name>eigen_mat</name>
      <anchorfile>namespacecyqlone_1_1qpalm_1_1problems.html</anchorfile>
      <anchor>a8aefe0780e8354abdf3e5e24c3e48128</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>LinearOCPStorage</type>
      <name>load_from_csv</name>
      <anchorfile>namespacecyqlone_1_1qpalm_1_1problems.html</anchorfile>
      <anchor>a46202fdbc35f05256b2b61be3fbfe8b6</anchor>
      <arglist>(const fs::path &amp;folder, const std::string &amp;name)</arglist>
    </member>
    <member kind="function">
      <type>PlatooningProblem</type>
      <name>platooning</name>
      <anchorfile>namespacecyqlone_1_1qpalm_1_1problems.html</anchorfile>
      <anchor>af4239cf33132e3cc8dc9be009df1350f</anchor>
      <arglist>(PlatooningParams p)</arglist>
    </member>
    <member kind="function">
      <type>SpringMassProblem</type>
      <name>spring_mass</name>
      <anchorfile>namespacecyqlone_1_1qpalm_1_1problems.html</anchorfile>
      <anchor>a593bba2e8cff6a0ff4383a4ac595c9f1</anchor>
      <arglist>(SpringMassParams p)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; eigen_mat, eigen_mat &gt;</type>
      <name>discretize_zoh</name>
      <anchorfile>namespacecyqlone_1_1qpalm_1_1problems.html</anchorfile>
      <anchor>afba022677ff29a053cf23e0a4720da11</anchor>
      <arglist>(const Eigen::Ref&lt; const eigen_mat &gt; &amp;A, const Eigen::Ref&lt; const eigen_mat &gt; &amp;B, real_t Ts)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; eigen_mat, eigen_mat, eigen_mat &gt;</type>
      <name>discretize_zoh</name>
      <anchorfile>namespacecyqlone_1_1qpalm_1_1problems.html</anchorfile>
      <anchor>ac4ba60b62c874e543030832b2dc7b78b</anchor>
      <arglist>(const Eigen::Ref&lt; const eigen_mat &gt; &amp;A, const Eigen::Ref&lt; const eigen_mat &gt; &amp;B, const Eigen::Ref&lt; const eigen_mat &gt; &amp;b, real_t Ts)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>topic-lin-solvers</name>
    <title>Linear solvers</title>
    <filename>group__topic-lin-solvers.html</filename>
    <subgroup>topic-block-tridiag-solvers</subgroup>
    <subgroup>topic-ocp-solvers</subgroup>
    <file>cyqlone.hpp</file>
  </compound>
  <compound kind="group">
    <name>topic-block-tridiag-solvers</name>
    <title>Block tridiagonal solvers</title>
    <filename>group__topic-block-tridiag-solvers.html</filename>
    <class kind="struct">cyqlone::TricyqleParams</class>
    <class kind="struct">cyqlone::TricyqleSolver</class>
    <member kind="enumeration">
      <type></type>
      <name>cyqlone::SolveMethod</name>
      <anchorfile>group__topic-block-tridiag-solvers.html</anchorfile>
      <anchor>ga922388e973fa665d5f5eb0c7d988c81f</anchor>
      <arglist></arglist>
      <enumvalue file="group__topic-block-tridiag-solvers.html" anchor="gga922388e973fa665d5f5eb0c7d988c81fa91fa024e7c3efdd56efd4588853cf488">StairPCG</enumvalue>
      <enumvalue file="group__topic-block-tridiag-solvers.html" anchor="gga922388e973fa665d5f5eb0c7d988c81fa811f52a4a3648918ef143a0cb6a6e334">JacobiPCG</enumvalue>
      <enumvalue file="group__topic-block-tridiag-solvers.html" anchor="gga922388e973fa665d5f5eb0c7d988c81fac0ff1599838f84547b6668a463198617">PCR</enumvalue>
    </member>
  </compound>
  <compound kind="group">
    <name>topic-ocp-solvers</name>
    <title>Optimal control solvers</title>
    <filename>group__topic-ocp-solvers.html</filename>
    <class kind="struct">cyqlone::CyqloneParams</class>
    <class kind="struct">cyqlone::CyqloneSolver</class>
  </compound>
  <compound kind="group">
    <name>topic-optimization-solvers</name>
    <title>Optimization solvers</title>
    <filename>group__topic-optimization-solvers.html</filename>
    <subgroup>topic-optimization-solvers-ocp</subgroup>
    <class kind="struct">cyqlone::qpalm::DetailedStats</class>
    <class kind="struct">cyqlone::qpalm::Settings</class>
    <class kind="struct">cyqlone::qpalm::SolverTimings</class>
    <class kind="struct">cyqlone::qpalm::SolverStats</class>
    <class kind="class">cyqlone::qpalm::Solver</class>
    <member kind="enumeration">
      <type></type>
      <name>cyqlone::qpalm::SolverStatus</name>
      <anchorfile>group__topic-optimization-solvers.html</anchorfile>
      <anchor>gade0917661457f4dc0d6396ded24277a4</anchor>
      <arglist></arglist>
      <enumvalue file="group__topic-optimization-solvers.html" anchor="ggade0917661457f4dc0d6396ded24277a4ad8a942ef2b04672adfafef0ad817a407">Busy</enumvalue>
      <enumvalue file="group__topic-optimization-solvers.html" anchor="ggade0917661457f4dc0d6396ded24277a4ada5418b1eb4e80d05f2c1c7c4fa1b6ec">Converged</enumvalue>
      <enumvalue file="group__topic-optimization-solvers.html" anchor="ggade0917661457f4dc0d6396ded24277a4a9e8b9350c8f1716701ec1f9bbe76432e">MaxTime</enumvalue>
      <enumvalue file="group__topic-optimization-solvers.html" anchor="ggade0917661457f4dc0d6396ded24277a4abbf52264f7a6e91c48a242f95aeed3db">MaxIter</enumvalue>
      <enumvalue file="group__topic-optimization-solvers.html" anchor="ggade0917661457f4dc0d6396ded24277a4aff6e97b110349dae9c6e8be0ff7a85c3">NotFinite</enumvalue>
      <enumvalue file="group__topic-optimization-solvers.html" anchor="ggade0917661457f4dc0d6396ded24277a4ab644b4f0de4cf0c46acb4583602e168c">NoProgress</enumvalue>
      <enumvalue file="group__topic-optimization-solvers.html" anchor="ggade0917661457f4dc0d6396ded24277a4a12c37c4278b8c0db6c2f65052569cd80">Interrupted</enumvalue>
      <enumvalue file="group__topic-optimization-solvers.html" anchor="ggade0917661457f4dc0d6396ded24277a4ab0d4998a26f5b5742ad38c4af8817e32">Exception</enumvalue>
    </member>
  </compound>
  <compound kind="group">
    <name>topic-optimization-solvers-ocp</name>
    <title>Optimal control solvers</title>
    <filename>group__topic-optimization-solvers-ocp.html</filename>
    <class kind="struct">cyqlone::qpalm::CyqloneData</class>
    <class kind="struct">cyqlone::qpalm::CyQPALMBackendSettings</class>
    <class kind="struct">cyqlone::qpalm::CyQPALMBackendStats</class>
    <class kind="struct">cyqlone::qpalm::unique_CyQPALMBackend</class>
    <member kind="enumeration">
      <type></type>
      <name>cyqlone::qpalm::WarmStartingStrategy</name>
      <anchorfile>group__topic-optimization-solvers-ocp.html</anchorfile>
      <anchor>gaeb4dd1f31c1ec8d3d9e0f9023da5cec3</anchor>
      <arglist></arglist>
      <enumvalue file="group__topic-optimization-solvers-ocp.html" anchor="ggaeb4dd1f31c1ec8d3d9e0f9023da5cec3a8b7a61a6e9bd5b8977b7b3f02a425adc">Zeros</enumvalue>
      <enumvalue file="group__topic-optimization-solvers-ocp.html" anchor="ggaeb4dd1f31c1ec8d3d9e0f9023da5cec3a5fb63579fc981698f97d55bfecb213ea">Copy</enumvalue>
      <enumvalue file="group__topic-optimization-solvers-ocp.html" anchor="ggaeb4dd1f31c1ec8d3d9e0f9023da5cec3a825a3d98017bab11815ad2817201324c">Shift</enumvalue>
      <enumvalue file="group__topic-optimization-solvers-ocp.html" anchor="ggaeb4dd1f31c1ec8d3d9e0f9023da5cec3ab882fa82c60e5b9a229e5ed6e52ad31a">ShiftNoInequality</enumvalue>
    </member>
    <member kind="function">
      <type>unique_CyQPALMBackend&lt; VL, DefaultOrder &gt;</type>
      <name>cyqlone::qpalm::make_cyqpalm_backend</name>
      <anchorfile>group__topic-optimization-solvers-ocp.html</anchorfile>
      <anchor>gae148b32eb392bb39d685929069cfabdb</anchor>
      <arglist>(const CyqloneStorage&lt; real_t &gt; &amp;ocp, CyqloneData data, const CyQPALMBackendSettings &amp;settings)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::qpalm::update_cyqpalm_backend</name>
      <anchorfile>group__topic-optimization-solvers-ocp.html</anchorfile>
      <anchor>gaf5dbe2437a5f7176b58b7743cba83d1c</anchor>
      <arglist>(CyQPALMBackend&lt; VL, DefaultOrder &gt; &amp;backend, const CyqloneStorage&lt; real_t &gt; &amp;ocp)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::qpalm::update_cyqpalm_backend</name>
      <anchorfile>group__topic-optimization-solvers-ocp.html</anchorfile>
      <anchor>ga8aac8ebf6f85aba82b32a927bc9bea00</anchor>
      <arglist>(CyQPALMBackend&lt; VL, DefaultOrder &gt; &amp;backend, const LinearOCPStorage &amp;ocp)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>topic-ocp-formulations</name>
    <title>Optimal control problem formulations</title>
    <filename>group__topic-ocp-formulations.html</filename>
    <file>conversion.hpp</file>
    <file>cyqlone-storage.hpp</file>
    <file>ocp.hpp</file>
    <class kind="struct">cyqlone::LinearOCPSparseQP</class>
    <class kind="struct">cyqlone::CyqloneStorage</class>
    <class kind="struct">cyqlone::OCPDim</class>
    <class kind="struct">cyqlone::LinearOCPStorage</class>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::reference_to_gradient</name>
      <anchorfile>group__topic-ocp-formulations.html</anchorfile>
      <anchor>ga61795701135500f6aaf49cc698e5976e</anchor>
      <arglist>(const LinearOCPStorage &amp;ocp, std::span&lt; const real_t &gt; ref, std::span&lt; real_t &gt; qr)</arglist>
    </member>
    <member kind="function">
      <type>LinearOCPStorage</type>
      <name>cyqlone::generate_random_ocp</name>
      <anchorfile>group__topic-ocp-formulations.html</anchorfile>
      <anchor>gafc61f634e110905af0dffeb3e27a4fd8</anchor>
      <arglist>(OCPDim dim, uint_fast32_t seed=0)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>topic-parallelization</name>
    <title>Parallelization</title>
    <filename>group__topic-parallelization.html</filename>
    <file>barrier.hpp</file>
    <file>parallel.hpp</file>
    <class kind="struct">cyqlone::EmptyCompletion</class>
    <class kind="class">cyqlone::TreeBarrier</class>
    <class kind="struct">cyqlone::parallel::SharedContext</class>
    <class kind="struct">cyqlone::parallel::Context</class>
  </compound>
  <compound kind="group">
    <name>topic-linalg</name>
    <title>Linear algebra</title>
    <filename>group__topic-linalg.html</filename>
    <subgroup>topic-linalg-flops</subgroup>
    <member kind="function">
      <type>norms&lt; simdified_value_t&lt; Vx &gt; &gt;::result</type>
      <name>cyqlone::linalg::norms_all</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga5b021d3e61d99e49576a574e7c919923</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>cyqlone::linalg::norm_inf</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga2332e39f282f0b22d60d5e0a57f4e932</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>cyqlone::linalg::norm_1</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga972ef7eb9a7e6a2c3680dfb95737e083</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>cyqlone::linalg::norm_2_squared</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga60918683b4f31e9f30b1b40fb12fdc97</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>cyqlone::linalg::norm_2</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gae11e8e17f4b74608291fe5b2d389869c</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>cyqlone::linalg::dot</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gac25c43a1c180bb6ad0faedbde44a20a9</anchor>
      <arglist>(Vx &amp;&amp;x, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::scale</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaeee444a2231976efc5f93a58cada45c5</anchor>
      <arglist>(T alpha, Vx &amp;&amp;x, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::scale</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga16a11c4519ab1e0cf4946c57bf4d0671</anchor>
      <arglist>(T alpha, Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::hadamard</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gae8ee607462b95f6106dc714e1fe86f60</anchor>
      <arglist>(Vx &amp;&amp;x, Vy &amp;&amp;y, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::hadamard</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga5f7dead7502413b423294c480fe67df7</anchor>
      <arglist>(Vx &amp;&amp;x, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::clamp</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gacd0f6a1148300c8895b047baafd80619</anchor>
      <arglist>(Vx &amp;&amp;x, Vlo &amp;&amp;lo, Vhi &amp;&amp;hi, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::clamp_resid</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga7c05b45f1fc5c69ed29a8cb7f0d4cd33</anchor>
      <arglist>(Vx &amp;&amp;x, Vlo &amp;&amp;lo, Vhi &amp;&amp;hi, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::axpby</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga7798d7d6e1ab0065fd4abc60fcc2e437</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Tb beta, Vy &amp;&amp;y, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::axpby</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga7be0c0501654d83ed8cc615b5d19d4e9</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Tb beta, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::axpy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga3f4f3b9f53f734e9a29368d36a5d80a0</anchor>
      <arglist>(Vy &amp;&amp;y, const std::array&lt; simdified_value_t&lt; Vy &gt;, sizeof...(Vx)&gt; &amp;alphas, Vx &amp;&amp;...x)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::axpy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaf92a45bb3dfc15de8dcd96ee9bd2345b</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Vy &amp;&amp;y, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::axpy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga6a42b0d6640680263a1c97afd7a6e2cb</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::negate</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga8107de760e8af9ee6dddc4f9878c1339</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, with_rotate_t&lt; Rotate &gt;={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::negate</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaae5b326a925859f5c11cddbd75b1b057</anchor>
      <arglist>(VA &amp;&amp;A, with_rotate_t&lt; Rotate &gt;={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::sub</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gafb26638c219b8754db25e88f69304439</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, VC &amp;&amp;C, with_rotate_t&lt; Rotate &gt;={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::sub</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga907ac42d74d99fe2a6abc9c4fcd80b2b</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, with_rotate_t&lt; Rotate &gt;={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::add</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga63e5d7edfbf2831545aea00d9a75966b</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, VC &amp;&amp;C, with_rotate_t&lt; Rotate &gt;={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::add</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gab6af7e9086aefaf875651e751e1ad92b</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, with_rotate_t&lt; Rotate &gt;={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::for_each_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga42110f9f447dd30af3fdc936a07d1a39</anchor>
      <arglist>(F &amp;&amp;fun, VA &amp;&amp;A, VAs &amp;&amp;...As)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::transform_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga5b35f1651072845d48695ccefa5ef549</anchor>
      <arglist>(F &amp;&amp;fun, VA &amp;&amp;A, VAs &amp;&amp;...As)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::transform2_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gacaef92140287b1c3e3c41c2a71ee69ab</anchor>
      <arglist>(F &amp;&amp;fun, VA &amp;&amp;A, VB &amp;&amp;B, VAs &amp;&amp;...As)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::transform_n_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaaa852f170922addceaa82a4cb63349c6</anchor>
      <arglist>(F &amp;&amp;fun, std::tuple&lt; VAs... &gt; As, VBs &amp;&amp;...Bs)</arglist>
    </member>
    <member kind="function">
      <type>norms&lt; simdified_value_t&lt; Vx &gt; &gt;::result</type>
      <name>cyqlone::linalg::multi::norms_all</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gade4eb9023d1b94d7657f68dc256860b8</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>cyqlone::linalg::multi::norm_inf</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga55c086fc7235a79e26c3aa5b1154c224</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>cyqlone::linalg::multi::norm_1</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gabf875f22442b0023c2142b00d0975554</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>cyqlone::linalg::multi::norm_2_squared</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gae934751791c316edde36013271945bf3</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>cyqlone::linalg::multi::norm_2</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga91b1ca93bfba644b53749eeac8edb444</anchor>
      <arglist>(Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>simdified_value_t&lt; Vx &gt;</type>
      <name>cyqlone::linalg::multi::dot</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gafac39321d4eca4894c8099e597a9b5f2</anchor>
      <arglist>(Vx &amp;&amp;x, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::multi::scale</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gad40329bb0eea1a12dd5b510935b93fca</anchor>
      <arglist>(T alpha, Vx &amp;&amp;x, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::multi::scale</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga4dd6dc0ecf27af5d2f74f12e0aaca26a</anchor>
      <arglist>(T alpha, Vx &amp;&amp;x)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::multi::hadamard</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga192f6d5da228f92a9e16216064fdac0e</anchor>
      <arglist>(Vx &amp;&amp;x, Vy &amp;&amp;y, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::multi::hadamard</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga970509394d0f01a224acb3eac0f97535</anchor>
      <arglist>(Vx &amp;&amp;x, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::multi::clamp</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaaea202adb9d942b00a786f902e45307d</anchor>
      <arglist>(Vx &amp;&amp;x, Vlo &amp;&amp;lo, Vhi &amp;&amp;hi, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::multi::clamp_resid</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga60b2783e234cd9c038d1067b4f529a38</anchor>
      <arglist>(Vx &amp;&amp;x, Vlo &amp;&amp;lo, Vhi &amp;&amp;hi, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::multi::axpby</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga2c0d5fb8f9ea81725e5d54d7605129e3</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Tb beta, Vy &amp;&amp;y, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::multi::axpby</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaca21e3c51e47359f16410a524f1a6896</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Tb beta, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::multi::axpy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaede77b28f5e46753ee7b915935f144b5</anchor>
      <arglist>(Vy &amp;&amp;y, const std::array&lt; simdified_value_t&lt; Vy &gt;, sizeof...(Vx)&gt; &amp;alphas, Vx &amp;&amp;...x)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::multi::axpy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga30451a0e4f4100069ff65963d1bb0dbf</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Vy &amp;&amp;y, Vz &amp;&amp;z)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::multi::axpy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga74efe6fd79de11b4fad66ce0444c2de3</anchor>
      <arglist>(Ta alpha, Vx &amp;&amp;x, Vy &amp;&amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::multi::negate</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga4024401b27cb17a5982d63dca91df61b</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, with_rotate_t&lt; Rotate &gt; rot={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::multi::negate</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gadfa94d9946d30ead1c8a0896599bd313</anchor>
      <arglist>(VA &amp;&amp;A, with_rotate_t&lt; Rotate &gt; rot={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::multi::sub</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaff7f734df0a2b041994f963e2226f858</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, VC &amp;&amp;C, with_rotate_t&lt; Rotate &gt; rot={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::multi::sub</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga48d07be903d60f6d48b7c3af0e9da414</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, with_rotate_t&lt; Rotate &gt; rot={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::multi::add</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gac534f2ffcc1821dedd98afd67b4d5e27</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, VC &amp;&amp;C, with_rotate_t&lt; Rotate &gt; rot={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::multi::add</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga84c12f9e93355fcc36107e18086ef7a4</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, with_rotate_t&lt; Rotate &gt; rot={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::multi::for_each_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gaa72458de29526c1deb6a0519926b2cd0</anchor>
      <arglist>(F &amp;&amp;fun, VA &amp;&amp;A, VAs &amp;&amp;...As)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::multi::transform_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga04fa9fa6e82563be75524b11f135bc2b</anchor>
      <arglist>(F &amp;&amp;fun, VA &amp;&amp;A, VAs &amp;&amp;...As)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::multi::transform2_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga79cf0bb8d436a1cba7ce70fab44bfbf0</anchor>
      <arglist>(F &amp;&amp;fun, VA &amp;&amp;A, VB &amp;&amp;B, VAs &amp;&amp;...As)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::multi::transform_n_elementwise</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga52c5beff69e8bdfb213cd47caacc104f</anchor>
      <arglist>(F &amp;&amp;fun, std::tuple&lt; VAs... &gt; As, VBs &amp;&amp;...Bs)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::multi::copy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga85860ca1111492777572a2ba091cf5ca</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B, Opts... opts)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::multi::copy</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga2e50e5f96654eb673d29ac8461143e91</anchor>
      <arglist>(Structured&lt; VA, S &gt; A, Structured&lt; VB, S &gt; B, Opts... opts)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::unpack</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>ga526900bf6322e7c42895c80c36ac2f5e</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::linalg::pack</name>
      <anchorfile>group__topic-linalg.html</anchorfile>
      <anchor>gae29d94393be39ebcd6ce5f909e28787c</anchor>
      <arglist>(VA &amp;&amp;A, VB &amp;&amp;B)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>topic-utilities</name>
    <title>Utilities</title>
    <filename>group__topic-utilities.html</filename>
    <subgroup>topic-util-matio</subgroup>
    <file>neumaier.hpp</file>
    <file>reduce.hpp</file>
    <file>sparse.hpp</file>
    <class kind="class">cyqlone::NeumaierSum</class>
    <class kind="struct">cyqlone::norms</class>
    <class kind="struct">cyqlone::norms&lt; T, void &gt;</class>
    <class kind="struct">cyqlone::SparseMatrix</class>
    <class kind="struct">cyqlone::SparseMatrixBuilder</class>
  </compound>
  <compound kind="group">
    <name>topic-util-matio</name>
    <title>Exporting and loading .mat files</title>
    <filename>group__topic-util-matio.html</filename>
    <file>matio.hpp</file>
    <member kind="typedef">
      <type>::_mat_t</type>
      <name>cyqlone::mat_t</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gac7f041cab5ae3c1471243d06da6f6469</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>std::unique_ptr&lt; mat_t, int(*)(mat_t *)&gt;</type>
      <name>cyqlone::MatFilePtr</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gaceac871d1342196be7468d2bb3fcd386</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>cyqlone::MatioOpenMode</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga8e268a5d303978ce26bb366dd8db62e4</anchor>
      <arglist></arglist>
      <enumvalue file="group__topic-util-matio.html" anchor="gga8e268a5d303978ce26bb366dd8db62e4a7a1a5f3e79fdc91edf2f5ead9d66abb4">Read</enumvalue>
      <enumvalue file="group__topic-util-matio.html" anchor="gga8e268a5d303978ce26bb366dd8db62e4a1129c0e4d43f2d121652a7302712cff6">Write</enumvalue>
    </member>
    <member kind="function">
      <type>MatFilePtr</type>
      <name>cyqlone::open_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga9a8d93b3cfc96c16fa9c70b969388c50</anchor>
      <arglist>(const std::filesystem::path &amp;filename, MatioOpenMode mode=MatioOpenMode::Read)</arglist>
    </member>
    <member kind="function">
      <type>MatFilePtr</type>
      <name>cyqlone::create_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga5b9bfdc6a8d4f53838511888e6724cb8</anchor>
      <arglist>(const std::filesystem::path &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga937f13ae7ced1e23973c106d256995a0</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, float value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga42854ade7f449f8478e5e3015d800a30</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, double value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga98ba9fcc8ae30f52267a6ed391911aa5</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, short value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga318cacc90af5fd74e3949f88a3a0c7e3</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, int value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga3dd29ca4a01d166d48251c7a5ea8ce34</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, long value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gaea09b40b9de36a89d1c91e595170c7c5</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, long long value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga60956f86298592566c35039d92a5edc3</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, unsigned short value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gafecae51b359b38c163783aba289d7218</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, unsigned int value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga1537cff5aed44fd0e58c4fee61675161</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, unsigned long value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga07ddf7772fe145d98e1ebdf2d9677cc1</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, unsigned long long value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gaaf4aac1a36819fc07e647af186b19e76</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, guanaqo::MatrixView&lt; const double, index_t &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gad4f62a7347cceafe8574c7e741f991cf</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, guanaqo::MatrixView&lt; const float, index_t &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga7f4118c8f4b3b9fd0dccfc578989e2b6</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, batmat::matrix::View&lt; const double, index_t &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gadc1a730925581865e91ec60b5c2d1cc0</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, batmat::matrix::View&lt; const float, index_t &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga81a3620a6c3dabf0e6bfd8f56960c300</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::span&lt; const float &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga581b8a2280d7eff0b756086643618db8</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::span&lt; const double &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga8ce5583b813db30a7ff21fb2568a3b3b</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::span&lt; const unsigned short &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga2a4285ac5b76e2829f06405eeaa7f484</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::span&lt; const unsigned int &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gadeddae83552dc401d00f90d86df1e610</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::span&lt; const unsigned long &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gac2fda9834d3303e31d9fb598ef949809</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::span&lt; const unsigned long long &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gaeac3d6a0248c962a3d5b29a91a0d08d0</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::span&lt; const short &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gaa20a79941500e76a4777644e83df8aa0</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::span&lt; const int &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga9007be7514cd0af283eb395d3396acc2</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::span&lt; const long &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gafb115ff7b2443481e6480f95caa3aaa0</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::span&lt; const long long &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gae79d8af02b2c40cfa0b8edd01ff7fa94</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, const SparseMatrix &amp;matrix)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::add_to_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga043f9e47579e7bf1529ad7baa0c86a12</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, const LinearOCPStorage &amp;ocp)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::read_from_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga9811702bc5a357d1064eb05bff17d4b6</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, LinearOCPStorage &amp;ocp)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::read_from_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga8400d97982852012a2f51f369a0da870</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::vector&lt; float &gt; &amp;data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::read_from_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga6f27d652223be08a963b98316c4b7b45</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::vector&lt; double &gt; &amp;data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::read_from_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga6d5e9459f93f03c070fed01a07cb66ba</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::span&lt; float &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::read_from_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>ga710eb1987fa4f8b1fda296b05fdfc685</anchor>
      <arglist>(mat_t *mat, const std::string &amp;varname, std::span&lt; double &gt; data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyqlone::ocp_dump_mat</name>
      <anchorfile>group__topic-util-matio.html</anchorfile>
      <anchor>gaf1ab96eebb1218ba8573c6d61ec461bf</anchor>
      <arglist>(const std::filesystem::path &amp;filename, const LinearOCPStorage &amp;ocp)</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>examples</name>
    <title>Examples</title>
    <filename>examples.html</filename>
    <docanchor file="examples.html" title="docs/source/doxygen/examples.md">md_docs_2source_2doxygen_2examples</docanchor>
  </compound>
  <compound kind="page">
    <name>md_benchmarks_2cyqpalm_2README</name>
    <title>CyQPALM benchmarks</title>
    <filename>md_benchmarks_2cyqpalm_2README.html</filename>
  </compound>
  <compound kind="page">
    <name>algorithms</name>
    <title>Algorithms</title>
    <filename>algorithms.html</filename>
    <docanchor file="algorithms.html" title="docs/source/doxygen/algorithms.md">md_docs_2source_2doxygen_2algorithms</docanchor>
    <docanchor file="algorithms.html" title="Algorithm 1: Factorization of a single modified Riccati block column">algorithm-1</docanchor>
    <docanchor file="algorithms.html" title="Algorithm 2: Cyqlone factorization">algorithm-2</docanchor>
    <docanchor file="algorithms.html" title="Algorithm 3: Factorization update of a single modified Riccati block column">algorithm-3</docanchor>
    <docanchor file="algorithms.html" title="Algorithm 4: Cyqlone factorization updates">algorithm-4</docanchor>
    <docanchor file="algorithms.html" title="Algorithm 5: CR: Solution of a symmetric block-tridiagonal system using cyclic reduction">algorithm-5</docanchor>
    <docanchor file="algorithms.html" title="Algorithm 6: PCR: Solution of a symmetric block-tridiagonal system using parallel cyclic reduction">algorithm-6</docanchor>
    <docanchor file="algorithms.html" title="Algorithm 7: Periodic PCR factorization of a block-tridiagonal matrix">algorithm-7</docanchor>
    <docanchor file="algorithms.html" title="Algorithm 8: Periodic PCR factorization updates by a block-bidiagonal matrix">algorithm-8</docanchor>
  </compound>
  <compound kind="dir">
    <name>src/qpalm/include/cyqlone/qpalm/backends/backend-cyqlone</name>
    <path>src/qpalm/include/cyqlone/qpalm/backends/backend-cyqlone/</path>
    <filename>dir_35c28c2d068fa72095cfe229e39093c5.html</filename>
    <file>ineq-constr.tpp</file>
    <file>linalg.tpp</file>
    <file>linesearch.tpp</file>
    <file>solve.tpp</file>
  </compound>
  <compound kind="dir">
    <name>src/qpalm/include/cyqlone/qpalm/backends</name>
    <path>src/qpalm/include/cyqlone/qpalm/backends/</path>
    <filename>dir_5a761708a043e211fe4c2c5b381fa9bf.html</filename>
    <dir>src/qpalm/include/cyqlone/qpalm/backends/backend-cyqlone</dir>
    <file>ocp-backend-cyqlone.hpp</file>
    <file>ocp-backend-cyqlone.tpp</file>
  </compound>
  <compound kind="dir">
    <name>benchmarks</name>
    <path>benchmarks/</path>
    <filename>dir_13a41d9ad64fb9ae396cb953294783cf.html</filename>
    <dir>benchmarks/cyqpalm</dir>
  </compound>
  <compound kind="dir">
    <name>src/cyqlone</name>
    <path>src/cyqlone/</path>
    <filename>dir_44ad1395222d8604571a7c368d1497ac.html</filename>
    <dir>src/cyqlone/include</dir>
    <dir>src/cyqlone/src</dir>
  </compound>
  <compound kind="dir">
    <name>src/cyqlone/include/cyqlone</name>
    <path>src/cyqlone/include/cyqlone/</path>
    <filename>dir_883a88dfe2d80794a3038754b6505506.html</filename>
    <dir>src/cyqlone/include/cyqlone/implementation</dir>
    <file>barrier.hpp</file>
    <file>config.hpp</file>
    <file>conversion.hpp</file>
    <file>cyqlone-params.hpp</file>
    <file>cyqlone-storage.hpp</file>
    <file>cyqlone.hpp</file>
    <file>linalg.hpp</file>
    <file>matio.hpp</file>
    <file>neumaier.hpp</file>
    <file>ocp.hpp</file>
    <file>packing.hpp</file>
    <file>parallel.hpp</file>
    <file>random-ocp.hpp</file>
    <file>reduce.hpp</file>
    <file>sparse.hpp</file>
    <file>timing.hpp</file>
    <file>tracing.hpp</file>
  </compound>
  <compound kind="dir">
    <name>src/example-problems/include/cyqlone</name>
    <path>src/example-problems/include/cyqlone/</path>
    <filename>dir_396e3360067733e5f3810bf7f62aba56.html</filename>
    <dir>src/example-problems/include/cyqlone/qpalm</dir>
  </compound>
  <compound kind="dir">
    <name>src/qpalm/include/cyqlone</name>
    <path>src/qpalm/include/cyqlone/</path>
    <filename>dir_c6f75b28a14edee788b557d3d0164195.html</filename>
    <dir>src/qpalm/include/cyqlone/qpalm</dir>
  </compound>
  <compound kind="dir">
    <name>benchmarks/cyqpalm</name>
    <path>benchmarks/cyqpalm/</path>
    <filename>dir_a176c167fe9328c1c90f722060dd8e38.html</filename>
    <file>spring-mass.cpp</file>
  </compound>
  <compound kind="dir">
    <name>docs</name>
    <path>docs/</path>
    <filename>dir_49e56c817e5e54854c35e136979f97ca.html</filename>
    <dir>docs/source</dir>
  </compound>
  <compound kind="dir">
    <name>docs/source/doxygen</name>
    <path>docs/source/doxygen/</path>
    <filename>dir_0f66d13221df5a7c05ce5dcf2db3bc52.html</filename>
  </compound>
  <compound kind="dir">
    <name>src/example-problems</name>
    <path>src/example-problems/</path>
    <filename>dir_7de462e88404737134ea1126d9379241.html</filename>
    <dir>src/example-problems/include</dir>
    <dir>src/example-problems/src</dir>
  </compound>
  <compound kind="dir">
    <name>src/example-problems/include/cyqlone/qpalm/example-problems</name>
    <path>src/example-problems/include/cyqlone/qpalm/example-problems/</path>
    <filename>dir_07802eb0453c26920e26e60b2160a921.html</filename>
    <file>csv.hpp</file>
    <file>platooning.hpp</file>
    <file>spring-mass.hpp</file>
    <file>zoh.hpp</file>
  </compound>
  <compound kind="dir">
    <name>examples</name>
    <path>examples/</path>
    <filename>dir_d28a4824dc47e487b107a5db32ef43c4.html</filename>
    <file>solve-block-tridiagonal.cpp</file>
  </compound>
  <compound kind="dir">
    <name>src/cyqlone/include/cyqlone/implementation</name>
    <path>src/cyqlone/include/cyqlone/implementation/</path>
    <filename>dir_52b2d70e12b6b2dd0d9cd3d7cdbaa926.html</filename>
    <file>cr.tpp</file>
    <file>data.tpp</file>
    <file>factor.tpp</file>
    <file>indexing.tpp</file>
    <file>mat-vec.tpp</file>
    <file>pcg.tpp</file>
    <file>pcr.tpp</file>
    <file>riccati.tpp</file>
    <file>schur.tpp</file>
    <file>sparse.tpp</file>
    <file>update.tpp</file>
  </compound>
  <compound kind="dir">
    <name>src/qpalm/include/cyqlone/qpalm/implementation</name>
    <path>src/qpalm/include/cyqlone/qpalm/implementation/</path>
    <filename>dir_a866254f62e3a09dbc49bd760cf63499.html</filename>
    <file>algorithms.hpp</file>
    <file>breakpoint.hpp</file>
    <file>breakpoint.tpp</file>
    <file>linesearch.tpp</file>
    <file>solver.tpp</file>
  </compound>
  <compound kind="dir">
    <name>src/cyqlone/include</name>
    <path>src/cyqlone/include/</path>
    <filename>dir_9199c9bb5bfaaac5b12cd21eebec9449.html</filename>
    <dir>src/cyqlone/include/cyqlone</dir>
  </compound>
  <compound kind="dir">
    <name>src/example-problems/include</name>
    <path>src/example-problems/include/</path>
    <filename>dir_b0ad1b14a57645f86573ae64172f605e.html</filename>
    <dir>src/example-problems/include/cyqlone</dir>
  </compound>
  <compound kind="dir">
    <name>src/qpalm/include</name>
    <path>src/qpalm/include/</path>
    <filename>dir_3f2f9b6a483e8fb1fb5becef34cc0a81.html</filename>
    <dir>src/qpalm/include/cyqlone</dir>
  </compound>
  <compound kind="dir">
    <name>src/example-problems/include/cyqlone/qpalm</name>
    <path>src/example-problems/include/cyqlone/qpalm/</path>
    <filename>dir_d3e88c8add9a2a1c0df8e375b0e19fe8.html</filename>
    <dir>src/example-problems/include/cyqlone/qpalm/example-problems</dir>
  </compound>
  <compound kind="dir">
    <name>src/qpalm</name>
    <path>src/qpalm/</path>
    <filename>dir_4f447dd82efef87cc1e4609c8432bde2.html</filename>
    <dir>src/qpalm/include</dir>
    <dir>src/qpalm/src</dir>
  </compound>
  <compound kind="dir">
    <name>src/qpalm/include/cyqlone/qpalm</name>
    <path>src/qpalm/include/cyqlone/qpalm/</path>
    <filename>dir_2d5279b0a350446182c761e0ab245364.html</filename>
    <dir>src/qpalm/include/cyqlone/qpalm/backends</dir>
    <dir>src/qpalm/include/cyqlone/qpalm/implementation</dir>
    <file>detailed-stats.hpp</file>
    <file>qpalm.hpp</file>
    <file>settings.hpp</file>
    <file>solver.hpp</file>
    <file>status.hpp</file>
  </compound>
  <compound kind="dir">
    <name>docs/source</name>
    <path>docs/source/</path>
    <filename>dir_1ab31cb45db5038bdda41b72d8600e0e.html</filename>
    <dir>docs/source/doxygen</dir>
  </compound>
  <compound kind="dir">
    <name>src</name>
    <path>src/</path>
    <filename>dir_68267d1309a1af8e8297ef4c3efbcdba.html</filename>
    <dir>src/cyqlone</dir>
    <dir>src/example-problems</dir>
    <dir>src/qpalm</dir>
  </compound>
  <compound kind="dir">
    <name>src/cyqlone/src</name>
    <path>src/cyqlone/src/</path>
    <filename>dir_8eaa555bbb8cec0abad941c0585b576e.html</filename>
    <file>conversion.cpp</file>
    <file>cyqlone-storage.cpp</file>
    <file>cyqlone.cpp</file>
    <file>matio.cpp</file>
    <file>ocp.cpp</file>
    <file>tracing.cpp</file>
  </compound>
  <compound kind="dir">
    <name>src/example-problems/src</name>
    <path>src/example-problems/src/</path>
    <filename>dir_b7c7187cfc7c2dae6fb436abbbfac846.html</filename>
    <file>csv.cpp</file>
    <file>platooning.cpp</file>
    <file>spring-mass.cpp</file>
  </compound>
  <compound kind="dir">
    <name>src/qpalm/src</name>
    <path>src/qpalm/src/</path>
    <filename>dir_25fe273618eceb7408f086225a281458.html</filename>
    <file>breakpoint.cpp</file>
    <file>ocp-backend-cyqlone.cpp</file>
    <file>qpalm.cpp</file>
    <file>status.cpp</file>
  </compound>
  <compound kind="dir">
    <name>test</name>
    <path>test/</path>
    <filename>dir_13e138d54eb8818da29c3992edef070a.html</filename>
    <file>test-pcr.cpp</file>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>cyqlone</title>
    <filename>index.html</filename>
    <docanchor file="index.html">md_README</docanchor>
  </compound>
</tagfile>
