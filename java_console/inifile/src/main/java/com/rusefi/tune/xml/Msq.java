package com.rusefi.tune.xml;

import com.opensr5.ConfigurationImage;
import com.opensr5.ini.IniFileModel;
import com.opensr5.ini.field.IniField;
import com.rusefi.xml.XmlUtil;
import org.jetbrains.annotations.NotNull;

import javax.xml.bind.JAXBException;
import javax.xml.bind.annotation.*;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

@XmlRootElement
public class Msq {

    public List<Page> page = new ArrayList<>();

    private final VersionInfo versionInfo;

    public Msq() {
        versionInfo = new VersionInfo("rusEFI+2020");
    }

    @NotNull
    public static Msq valueOf(ConfigurationImage image, int totalConfigSize, String tsSignature) {
        IniFileModel ini = IniFileModel.getInstance();
        Msq tune = create(totalConfigSize, tsSignature);
        for (String key : ini.allIniFields.keySet())
            tune.loadConstant(ini, key, image);
        return tune;
    }

    @NotNull
    public static Msq create(int totalConfigSize, String tsSignature) {
        Msq tune = new Msq();
        tune.versionInfo.setTsSignature(tsSignature);
        tune.page.add(new Page(null, null));
        tune.page.add(new Page(0, totalConfigSize));
        return tune;
    }

    public ConfigurationImage asImage(IniFileModel instance, int totalConfigSize) {
        ConfigurationImage ci = new ConfigurationImage(totalConfigSize);

        Page page = findPage();
        if (page == null)
            return ci;
        for (Constant constant : page.constant) {
            if (constant.getName().startsWith("UNALLOCATED_SPACE")) {
                continue;
            }
            IniField field = instance.allIniFields.get(constant.getName());
            Objects.requireNonNull(field, "Field for " + constant.getName());
            System.out.println("Setting " + field);
            field.setValue(ci, constant);
        }
        return ci;
    }

    public static Msq readTune(String fileName) throws Exception {
        return XmlUtil.readModel(Msq.class, fileName);
    }

    public void writeXmlFile(String outputXmlFileName) throws JAXBException, IOException {
        XmlUtil.writeXml(this, Msq.class, outputXmlFileName);
    }

    public void loadConstant(IniFileModel ini, String key, ConfigurationImage image) {
        IniField field = ini.allIniFields.get(key);
        String value = field.getValue(image);
        Page page = findPage();
        if (page == null) {
            System.out.println("Msq: No page");
            return;
        }
        page.constant.add(new Constant(field.getName(), field.getUnits(), value));
    }

    @XmlElement
    public Bibliography getBibliography() {
        return new Bibliography();
    }

    @XmlElement
    public VersionInfo getVersionInfo() {
        return versionInfo;
    }

    public Page findPage() {
        for (Page p : page) {
            Integer size = p.getSize();
            if (size == null)
                continue;
            return p;
        }
        return null;
    }

    @XmlElement
    public Settings getSettings() {
        return new Settings();
    }

    @XmlElement
    public UserComments getUserComments() {
        return new UserComments();
    }
}
